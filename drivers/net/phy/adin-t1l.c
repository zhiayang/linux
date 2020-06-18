// SPDX-License-Identifier: GPL-2.0+
/**
 *  Driver for Analog Devices Industrial Ethernet T1L PHYs
 *
 * Copyright 2020 Analog Devices Inc.
 */
#include <linux/kernel.h>
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/property.h>

#include "adin-compat.h"

#define PHY_ID_ADIN1100				0x0283bc80

#define ADIN_T1L_FEATURES			(SUPPORTED_10baseT_Full)

#define ADIN_AN_LP_ADV_ABILITY_L		0x0205

#define ADIN_AN_LP_ADV_ABILITY_M		0x0206
#define   ADIN_AN_LP_ADV_B10L			BIT(14)
#define   ADIN_AN_LP_ADV_B1000			BIT(7)
#define   ADIN_AN_LP_ADV_B10S_FD		BIT(6)
#define   ADIN_AN_LP_ADV_B100			BIT(5)
#define   ADIN_AN_LP_ADV_MST			BIT(4)

#define ADIN_AN_LP_ADV_ABILITY_H		0x0207
#define   ADIN_AN_LP_ADV_B10L_EEE		BIT(14)
#define   ADIN_AN_LP_ADV_B10L_TX_LVL_HI_ABL	BIT(13)
#define   ADIN_AN_LP_ADV_B10L_TX_LVL_HI_REQ	BIT(12)
#define   ADIN_AN_LP_ADV_B10S_HD		BIT(11)

/**
 * The ADIN T1L PHY supports Clause 45-only access, but is not a C45 device
 * in the strictest sense of how Linux considers C45 capable devices.
 *
 * For once, it doesn't support C45 package IDs, so it won't probe
 * via the normal get_phy_c45_devs_in_pkg() path.
 * Secondly, the chip implements mostly 10BASE-T1L regs, so autonegotiation
 * requires some other regs which are IEEE spec-ed, but not standard in Linux.
 *
 */

static u32 adin_mdio_addr_xlate(int devad, u16 regnum)
{
	u32 addr = MII_ADDR_C45 | (devad << 16) | (regnum & 0xffff);

	switch (devad) {
	/**
	 * BASE-T1L auto-negotiation regs start at 0x0200.
	 * The Control & Status bitfields are similar to standard regs.
	 */
	case MDIO_MMD_AN:
		switch (regnum) {
		case MDIO_STAT1:
		case MDIO_CTRL1:
			addr |= 0x0200;
			break;
		}
	}

	return addr;
}

static int adin_read_mmd(struct phy_device *phydev, int devad, u16 regnum)
{
	u32 addr = adin_mdio_addr_xlate(devad, regnum);

	return __mdiobus_read(phydev->mdio.bus, phydev->mdio.addr, addr);
}

static int adin_write_mmd(struct phy_device *phydev, int devad, u16 regnum,
			  u16 val)
{
	u32 addr = adin_mdio_addr_xlate(devad, regnum);

	return __mdiobus_write(phydev->mdio.bus, phydev->mdio.addr, addr, val);
}

static int adin_match_phy_device(struct phy_device *phydev)
{
	u32 id;
	int rc;

	/**
	 * Need to call adin_read_mmd() directly here, because at this point
	 * in time, the driver isn't attached to the PHY device.
	 */
	rc = adin_read_mmd(phydev, MDIO_MMD_PMAPMD, MDIO_DEVID1);
	if (rc < 0)
		return rc;

	id = rc << 16;

	rc = adin_read_mmd(phydev, MDIO_MMD_PMAPMD, MDIO_DEVID2);
	if (rc < 0)
		return rc;

	id |= rc;

	switch (id) {
	case PHY_ID_ADIN1100:
		return 1;
	default:
		return 0;
	}
}

static u32 adin_mii_adv_m_to_ethtool_adv_t(u32 adv)
{
	u32 result = 0;

	if (adv & ADIN_AN_LP_ADV_B10L)
		result |= ADVERTISED_10baseT_Full;
	if (adv & ADIN_AN_LP_ADV_B1000) {
		result |= ADVERTISED_1000baseT_Half;
		result |= ADVERTISED_1000baseT_Full;
	}
	if (adv & ADIN_AN_LP_ADV_B10S_FD)
		result |= ADVERTISED_10baseT_Full;
	if (adv & ADIN_AN_LP_ADV_B100)
		result |= ADVERTISED_100baseT_Full;

	return result;
}

static inline u32 adin_mii_adv_h_to_ethtool_adv_t(u32 adv)
{
	u32 result = 0;

	if (adv & ADIN_AN_LP_ADV_B10S_HD)
		result |= ADVERTISED_10baseT_Half;

	return result;
}

static inline int adin_read_lpa(struct phy_device *phydev)
{
	int val;

	phydev->lp_advertising = 0;

	val = phy_read_mmd(phydev, MDIO_MMD_AN, MDIO_STAT1);
	if (val < 0)
		return val;

	if (!(val & MDIO_AN_STAT1_COMPLETE)) {
                phydev->pause = 0;
                phydev->asym_pause = 0;

                return 0;
        }

	phydev->lp_advertising |= ADVERTISED_Autoneg;

	/* Read the link partner's base page advertisement */
	val = phy_read_mmd(phydev, MDIO_MMD_AN, ADIN_AN_LP_ADV_ABILITY_L);
	if (val < 0)
		return val;

	phydev->pause = val & LPA_PAUSE_CAP ? 1 : 0;
	phydev->asym_pause = val & LPA_PAUSE_ASYM ? 1 : 0;

	val = phy_read_mmd(phydev, MDIO_MMD_AN, ADIN_AN_LP_ADV_ABILITY_M);
	if (val < 0)
		return val;

	phydev->lp_advertising |= adin_mii_adv_m_to_ethtool_adv_t(val);

	val = phy_read_mmd(phydev, MDIO_MMD_AN, ADIN_AN_LP_ADV_ABILITY_H);
	if (val < 0)
		return val;

	phydev->lp_advertising |= adin_mii_adv_h_to_ethtool_adv_t(val);

	return 0;
}

static int adin_read_status(struct phy_device *phydev)
{
	int ret;

	ret = genphy_c45_read_link(phydev);
	if (ret)
		return ret;

	phydev->speed = SPEED_UNKNOWN;
	phydev->duplex = DUPLEX_UNKNOWN;
	phydev->pause = 0;
	phydev->asym_pause = 0;

	if (phydev->autoneg == AUTONEG_ENABLE) {
		ret = adin_read_lpa(phydev);
		if (ret)
			return ret;

		phy_resolve_aneg_linkmode(phydev);
	} else {
		/**
		 * The standard C45 PMA regs read zeros here, and this PHY
		 * can only do one speed & duplex anyway
		 */
		phydev->lp_advertising = 0;
		phydev->speed = SPEED_10;
		phydev->duplex = DUPLEX_FULL;
	}

	return ret;
}

static int adin_config_aneg(struct phy_device *phydev)
{
	/* FIXME: don't allow autonegotiation disable for now; link will stop working */
	if (phydev->autoneg == AUTONEG_DISABLE)
		return 0;

	/*
	 * Only 10 Mbit Full-Duplex is supported, so just restart auto-neg.
	 * The genphy_c45_check_and_restart_aneg() should restart if needed.
	 */

	return genphy_c45_check_and_restart_aneg(phydev, false);
}

static int adin_config_init(struct phy_device *phydev)
{
	phydev->supported = SUPPORTED_10baseT_Full;
	phydev->advertising = SUPPORTED_10baseT_Full;

	return 0;
}

static struct phy_driver adin_driver[] = {
	{
		PHY_ID_MATCH_MODEL(PHY_ID_ADIN1100),
		.name			= "ADIN1100",
		.features		= ADIN_T1L_FEATURES, /* FIXME: remove this when the `get_features` hook becomes available */
		.match_phy_device	= adin_match_phy_device,
		.config_init		= adin_config_init,
		.config_aneg		= adin_config_aneg,
		.aneg_done		= genphy_c45_aneg_done,
		.read_status		= adin_read_status,
		.read_mmd		= adin_read_mmd,
		.write_mmd		= adin_write_mmd,
	},
};

module_phy_driver(adin_driver);

static struct mdio_device_id __maybe_unused adin_tbl[] = {
	{ PHY_ID_MATCH_MODEL(PHY_ID_ADIN1100) },
	{ }
};

MODULE_DEVICE_TABLE(mdio, adin_tbl);
MODULE_DESCRIPTION("Analog Devices Industrial Ethernet T1L PHY driver");
MODULE_LICENSE("GPL");
