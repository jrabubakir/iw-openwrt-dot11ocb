#include <errno.h>
#include <string.h>

#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include "nl80211.h"
#include "iw.h"

static int set_dot11ocb_state(struct nl80211_state *state,
			  struct nl_cb *cb,
			  struct nl_msg *msg,
			  int argc, char **argv,
			  enum id_input id)
{
	bool dot11ocbenabled;

	if (argc != 1) {
		printf("Invalid parameters!\n");
		return 2;
	}

	if (strcmp(argv[0], "enable") == 0)
		dot11ocbenabled = true;
	else if (strcmp(argv[0], "disable") == 0)
		dot11ocbenabled = false;
	else {
		printf("Invalid parameter: %s\n", argv[0]);
		return 2;
	}

	NLA_PUT_U32(msg, NL80211_ATTR_DOT11OCB_STATE, dot11ocbenabled);

	return 0;

 nla_put_failure:
	return -ENOBUFS;
}

COMMAND(set, dot11ocb, "<enable|disable>",
	NL80211_CMD_SET_DOT11OCB, 0, CIB_NETDEV, set_dot11ocb_state,
	"Enable/disable .11OCB mode");

static int print_dot11ocb_state(struct nl_msg *msg, void *arg)
{
	struct nlattr *attrs[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	const char *s;

	nla_parse(attrs, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!attrs[NL80211_ATTR_DOT11OCB_STATE]) {
		printf("There is no NL80211_ATTR_DOT11OCB_STATE attribute");
		return NL_SKIP;
	}

	switch (nla_get_u32(attrs[NL80211_ATTR_DOT11OCB_STATE])) {
	case 1:
		s = "enabled";
		break;
	case 0:
	default:
		s = "disabled";
		break;
	}

	printf(".11OCB mode: %s\n", s);

	return NL_SKIP;
}

static int get_dot11ocb_state(struct nl80211_state *state,
				   struct nl_cb *cb,
				   struct nl_msg *msg,
				   int argc, char **argv,
				   enum id_input id)
{
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM,
		  print_dot11ocb_state, NULL);
	return 0;
}

COMMAND(get, dot11ocb, "",
	NL80211_CMD_GET_DOT11OCB, 0, CIB_NETDEV, get_dot11ocb_state,
	"Retrieve .11OCB mode.");
