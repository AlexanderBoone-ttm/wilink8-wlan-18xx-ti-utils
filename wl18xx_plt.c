/*
 * This file is part of calibrator
 *
 * Copyright (C) 2011 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "calibrator.h"
#include "plt.h"
#include "wl18xx_plt.h"

SECTION(wl18xx_plt);


static int  plt_wl18xx_phy_reg_write(struct nl80211_state *state, struct nl_cb *cb,
			      struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_phy_reg_write prms;
	struct fw_version fw_ver_valid={{8,4,0,0,2}};

	/* Validate the FW supported version (given 3 args before) */
	if (is_fw_ver_valid(*(argv-3), &fw_ver_valid)) {
		printf("\tFail: the FW version does not support this command\n");
		printf("\tThe FW supported verstion starting from %d.%d.%d.%d.%d\n",
				fw_ver_valid.ver[0], fw_ver_valid.ver[1],
				fw_ver_valid.ver[2], fw_ver_valid.ver[3],
				fw_ver_valid.ver[4]);
		return 1;
	}

	if (argc != 2)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_PHY_ADDR_WRITE;


	prms.addr 	= strtol(argv[0], NULL, 16);
	prms.data	= strtol(argv[1], NULL, 16);

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, phy_reg_write , "<addr> <data> ",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_phy_reg_write,
	" Write PHY register for PLT.\n");

static int plt_wl18xx_display_phy_reg_read(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *td[WL1271_TM_ATTR_MAX + 1];
	struct wl18xx_cmd_phy_reg_read *prms;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_TESTDATA]) {
		fprintf(stderr, "no data!\n");
		return NL_SKIP;
	}

	nla_parse(td, WL1271_TM_ATTR_MAX, nla_data(tb[NL80211_ATTR_TESTDATA]),
		  nla_len(tb[NL80211_ATTR_TESTDATA]), NULL);

	prms = (struct wl18xx_cmd_phy_reg_read *) nla_data(td[WL1271_TM_ATTR_DATA]);

	printf("Register Address: \t0x%x\t", prms->addr);
	printf("is:\t0x%x\n", prms->data);

	return NL_SKIP;
}


static int  plt_wl18xx_phy_reg_read(struct nl80211_state *state, struct nl_cb *cb,
			      struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_phy_reg_read prms;
	struct fw_version fw_ver_valid={{8,4,0,0,2}};

	/* Validate the FW supported version (given 3 args before) */
	if (is_fw_ver_valid(*(argv-3), &fw_ver_valid)) {
		printf("\tFail: the FW version does not support this command\n");
		printf("\tThe FW supported verstion starting from %d.%d.%d.%d.%d\n",
				fw_ver_valid.ver[0], fw_ver_valid.ver[1],
				fw_ver_valid.ver[2], fw_ver_valid.ver[3],
				fw_ver_valid.ver[4]);
		return 1;
	}

	if (argc != 1)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_PHY_ADDR_READ;

	prms.addr 	= strtol(argv[0], NULL, 16);

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);
	NLA_PUT_U8(msg, WL1271_TM_ATTR_ANSWER, 1);

	nla_nest_end(msg, key);

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM,
			plt_wl18xx_display_phy_reg_read, NULL);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, phy_reg_read , "<addr>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_phy_reg_read,
	" Read PHY register for PLT.\n");


static int  plt_wl18xx_set_antenna_mode_5G(struct nl80211_state *state, struct nl_cb *cb,
			      struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_set_antenna_mode_5G prms;
	struct fw_version fw_ver_valid={{8,4,0,0,2}};

	/* Validate the FW supported version (given 3 args before) */
	if (is_fw_ver_valid(*(argv-3), &fw_ver_valid)) {
		printf("\tFail: the FW version does not support this command\n");
		printf("\tThe FW supported verstion starting from %d.%d.%d.%d.%d\n",
				fw_ver_valid.ver[0], fw_ver_valid.ver[1],
				fw_ver_valid.ver[2], fw_ver_valid.ver[3],
				fw_ver_valid.ver[4]);
		return 1;
	}


	if (argc != 4)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_SET_ANTENNA_MODE_5G;


	prms.mac_prim_rx_chain = (__u8)atoi(argv[0]);
	prms.mac_rx_chain1_en  = (__u8)atoi(argv[1]);
	prms.mac_rx_chain2_en  = (__u8)atoi(argv[2]);
	prms.mac_tx_chain1_en  = (__u8)atoi(argv[3]);

	if ((prms.mac_prim_rx_chain != 1 && prms.mac_prim_rx_chain != 2 ))
		return 1;

	if ((prms.mac_rx_chain1_en > 1) || (prms.mac_rx_chain2_en > 1)
			|| (prms.mac_tx_chain1_en > 1))
		return 1;

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, set_antenna_mode_5G , "<mac_prim_rx_chain> <mac_rx_chain1_en> "
		"<mac_rx_chain2_en> <mac_tx_chain1_en>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_set_antenna_mode_5G,
	"set antenna mode 5G for PLT.\n");


static int  plt_wl18xx_set_antenna_mode_24G(struct nl80211_state *state, struct nl_cb *cb,
			      struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_set_antenna_mode_24G prms;
	struct fw_version fw_ver_valid={{8,4,0,0,2}};

	/* Validate the FW supported version (given 3 args before) */
	if (is_fw_ver_valid(*(argv-3), &fw_ver_valid)) {
		printf("\tFail: the FW version does not support this command\n");
		printf("\tThe FW supported verstion starting from %d.%d.%d.%d.%d\n",
				fw_ver_valid.ver[0], fw_ver_valid.ver[1],
				fw_ver_valid.ver[2], fw_ver_valid.ver[3],
				fw_ver_valid.ver[4]);
		return 1;
	}

	if (argc != 6)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_SET_ANTENNA_MODE_24G;


	prms.mac_prim_rx_chain = (__u8)atoi(argv[0]);
	prms.mac_prim_tx_chain = (__u8)atoi(argv[1]);
	prms.mac_rx_chain1_en  = (__u8)atoi(argv[2]);
	prms.mac_rx_chain2_en  = (__u8)atoi(argv[3]);
	prms.mac_tx_chain1_en  = (__u8)atoi(argv[4]);
	prms.mac_tx_chain2_en  = (__u8)atoi(argv[5]);

	if ((prms.mac_prim_rx_chain != 1 && prms.mac_prim_rx_chain != 2 ) ||
		(prms.mac_prim_tx_chain != 1 && prms.mac_prim_tx_chain != 2 ))
		return 1;

	if ((prms.mac_rx_chain1_en > 1) || (prms.mac_rx_chain2_en > 1) ||
		(prms.mac_tx_chain1_en > 1) || (prms.mac_tx_chain2_en > 1))
		return 1;

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, set_antenna_mode_24G , "<mac_prim_rx_chain> <mac_prim_tx_chain> "
		"<mac_rx_chain1_en> <mac_rx_chain2_en> <mac_tx_chain1_en> <mac_tx_chain2_en>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_set_antenna_mode_24G,
	"set antenna mode 2.4G for PLT.\n");

static int plt_wl18xx_set_tx_power(struct nl80211_state *state,
		struct nl_cb *cb, struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_set_tx_power prms;

	if (argc != 12)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_TX_POWER;

	prms.mac_des_pwr = atoi(argv[0]);
	prms.mac_lvl_idx = atoi(argv[1]);
	prms.freq_band = atoi(argv[2]);
	prms.freq_prim_chan_num = atoi(argv[3]);
	prms.freq_2nd_chan_idx = atoi(argv[4]);
	prms.mac_ant_select = atoi(argv[5]);
	prms.mac_non_srv = atoi(argv[6]);
	prms.mac_chan_lim_dis = atoi(argv[7]);
	prms.mac_fem_lim_dis = atoi(argv[8]);
	prms.mac_gain_calc_mode = atoi(argv[9]);
	prms.mac_analog_gain_control_idx = atoi(argv[10]);
	prms.mac_post_dpd_gain = atoi(argv[11]);

	if (prms.mac_lvl_idx > 3)
		return 1;

	if (prms.freq_band > 2)
		return 1;

	if (prms.freq_prim_chan_num <= 0)
		return 1;

	if (prms.mac_ant_select > 3)
		return 1;

	if (prms.mac_non_srv > 1)
		return 1;

	if (prms.mac_chan_lim_dis > 1)
		return 1;

	if (prms.mac_fem_lim_dis > 1)
		return 1;

	if (prms.mac_gain_calc_mode > 2)
		return 1;

	if (prms.mac_analog_gain_control_idx > 4)
		return 1;

	if (prms.mac_post_dpd_gain > 4)
		return 1;

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, set_tx_power, "<output_power> <level> <band> "
		"<primary_channel> <2nd_channel> <antenna> <non_serving_channel> "
		"<channel_limitation> <frontend_limit> <gain_calculation> "
		"<analog_gain_control_id> <post_dpd_gain>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_set_tx_power,
	"set TX transmissions power for PLT.\n");

static int plt_wl18xx_tune_channel(struct nl80211_state *state, struct nl_cb *cb,
				   struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_channel_tune prms;

	if (argc != 3)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_CHANNEL_TUNE;
	prms.channel	= (__u8)atoi(argv[0]);
	prms.band	= (__u8)atoi(argv[1]);
	prms.bandwidth	= (__u8)atoi(argv[2]);

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, tune_channel, "<channel> <band> <bandwidth>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_tune_channel,
	"Set channel, band and bandwidth for PLT.\n");

#define RX_FILTER_MAGIC_NUMBER 0xabadabad

static int plt_wl18xx_start_rx(struct nl80211_state *state, struct nl_cb *cb,
			       struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_start_rx prms;

	if (argc > 2)
		return 1;

	prms.test.id    = WL18XX_TEST_CMD_START_RX_SIMULATION;

	if (argc == 2) {
		str2mac(prms.src_addr, argv[0]);
		str2mac(prms.dst_addr, argv[1]);
		prms.magic_num = RX_FILTER_MAGIC_NUMBER;
	}

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, start_rx, "<source address> <destination address>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_start_rx,
	"Start gathering RX statistics for PLT.\n");

static int plt_wl18xx_stop_rx(struct nl80211_state *state, struct nl_cb *cb,
			      struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_stop_rx prms;

	if (argc != 0)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_STOP_RX_SIMULATION;

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, stop_rx, "",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_stop_rx,
	"Stop gathering RX statistics for PLT.\n");

static int plt_wl18xx_display_rx_stats(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *td[WL1271_TM_ATTR_MAX + 1];
	struct wl18xx_cmd_rx_stats *prms;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_TESTDATA]) {
		fprintf(stderr, "no data!\n");
		return NL_SKIP;
	}

	nla_parse(td, WL1271_TM_ATTR_MAX, nla_data(tb[NL80211_ATTR_TESTDATA]),
		  nla_len(tb[NL80211_ATTR_TESTDATA]), NULL);

	prms = (struct wl18xx_cmd_rx_stats *) nla_data(td[WL1271_TM_ATTR_DATA]);

	printf("\nRX statistics (status %d)\n", prms->radio_status);
	printf("Total packets:\t%d\n", prms->total);
	printf("FCS errors:\t%d\n", prms->errors);
	printf("MAC mismatch:\t%d\n", prms->addr_mm);
	printf("Good packets:\t%d\n", prms->good);

	return NL_SKIP;
}

static int plt_wl18xx_get_rx_stats(struct nl80211_state *state, struct nl_cb *cb,
				   struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_rx_stats prms;

	if (argc != 0)
		return 1;

	prms.test.id = WL18XX_TEST_CMD_GET_RX_STATISTICS;

	printf("test.id = 0x%0x\n", prms.test.id);

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);
	NLA_PUT_U8(msg, WL1271_TM_ATTR_ANSWER, 1);

	nla_nest_end(msg, key);

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM,
		  plt_wl18xx_display_rx_stats, NULL);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, get_rx_stats, "",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_get_rx_stats,
	"Retrieve RX statistics for PLT.\n");

static int plt_wl18xx_start_tx(struct nl80211_state *state, struct nl_cb *cb,
			       struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_start_tx prms;

	if (argc != 11)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_START_TX_SIMULATION;

	prms.delay = atoi(argv[0]);
	prms.rate = atoi(argv[1]);
	prms.size = atoi(argv[2]);
	prms.mode = atoi(argv[3]);
	prms.data_type = atoi(argv[4]);
	prms.gi = atoi(argv[5]);
	prms.options1 = atoi(argv[6]);
	prms.options2 = atoi(argv[7]);
	str2mac(prms.src_addr, argv[8]);
	str2mac(prms.dst_addr, argv[9]);
	prms.bandwidth = atoi(argv[10]);

	if (prms.rate < 0 || prms.rate > 29)
		return 1;
	if (prms.gi != 0 && prms.gi != 1)
		return 1;
	if (prms.bandwidth != 0 && prms.bandwidth != 1)
		return 1;

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, start_tx, "<delay> <rate> <size> <mode> <data_type> <gi> "
	"<options1> <options2> <source MAC> <dest MAC> <20|40>",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_start_tx,
	"Start TX transmissions for PLT.\n\n"
	"<delay>\t\tdelay between packets in us\n"
	"<rate>\t\ttransmission rate:\n"
	"\t\t\t0  =  1.0 Mbps\t\t\t1  =  2.0 Mbps\n"
	"\t\t\t2  =  5.0 Mbps\t\t\t3  = 11.0 Mbps\n"
	"\t\t\t4  =  6.0 Mbps\t\t\t5  =  9.0 Mbps\n"
	"\t\t\t6  = 12.0 Mbps\t\t\t7  = 18.0 Mbps\n"
	"\t\t\t8  = 24.0 Mbps\t\t\t9  = 36.0 Mbps\n"
	"\t\t\t10 = 48.0 Mbps\t\t\t11 = 54.0 Mbps\n"
	"\t\t\t12 =  6.5 Mbps (MCS0)\t\t13 = 13.0 Mbps (MCS1)\n"
	"\t\t\t14 = 19.5 Mbps (MCS2)\t\t15 = 26.0 Mbps (MCS3)\n"
	"\t\t\t16 = 39.0 Mbps (MCS4)\t\t17 = 52.0 Mbps (MCS5)\n"
	"\t\t\t18 = 58.5 Mbps (MCS6)\t\t19 = 65.0 Mbps (MCS7)\n"
	"\t\t\t20 = 65.0 Mbps + 10% (MCS7 SGI)\t21 = MCS8/MCS4  at 40MHz\n"
	"\t\t\t22 = MCS9/MCS5  at 40MHz\t23 = MCS10/MCS6 at 40MHz\n"
	"\t\t\t24 = MCS11/MCS7 at 40MHz\t25 = MCS12/MCS7 at 40MHz SGI\n"
	"\t\t\t26 = MCS13\t\t\t27 = MCS14\n"
	"\t\t\t28 = MCS15\t\t\t29 = MCS15 SGI\n"
	"<size>\t\tpacket size (bytes)\n"
	"<mode>\t\tnumber of packets (0 = endless)\n"
	"<data_type>\tTBD\n"
	"<gi>\t\tguard interval (0 = normal, 1 = short)\n"
	"<options1>\tTBD\n"
	"<options2>\tTBD\n"
	"<source MAC>\tsource MAC address (XX:XX:XX:XX:XX:XX)\n"
	"<dest MAC>\tdestination MAC address (XX:XX:XX:XX:XX:XX)\n"
	"<channel width>\tchannel width (0 = 20 MHz, 1 = 40 MHz)");

static int plt_wl18xx_stop_tx(struct nl80211_state *state, struct nl_cb *cb,
			      struct nl_msg *msg, int argc, char **argv)
{
	struct nlattr *key;
	struct wl18xx_cmd_stop_tx prms;

	if (argc != 0)
		return 1;

	prms.test.id	= WL18XX_TEST_CMD_STOP_TX_SIMULATION;

	key = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!key) {
		fprintf(stderr, "fail to nla_nest_start()\n");
		return 1;
	}

	NLA_PUT_U32(msg, WL1271_TM_ATTR_CMD_ID, WL1271_TM_CMD_TEST);
	NLA_PUT(msg, WL1271_TM_ATTR_DATA, sizeof(prms), &prms);

	nla_nest_end(msg, key);

	return 0;

nla_put_failure:
	fprintf(stderr, "%s> building message failed\n", __func__);
	return 2;
}

COMMAND(wl18xx_plt, stop_tx, "",
	NL80211_CMD_TESTMODE, 0, CIB_NETDEV, plt_wl18xx_stop_tx,
	"Stop TX transmissions for PLT.\n");

