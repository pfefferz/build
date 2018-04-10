/* decode.c
 *
 * The MIT License (MIT)
 *
 * Copyright(c) 2018 Centennial Software Solutions LLC.
 *
 * inquiries@centennialsoftwaresolutions.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/*
 * Build with:
 * make decode
 * or
 * make CFLAGS="-Wall" decode
 *
 * Call with ./decode reglog.txt
 *
 * reglog.txt (or the input file) just needs lines like this:
 * Device 0x10 Registers:
 * 0x00 -> 0x0009
 * 0x01 -> 0x0003
 * 0x02 -> 0xFF00
 * 0x03 -> 0x3102 Etc...
 *
 * To look at all the registers that may change:
 * ./decode reglog.txt | grep -E 'RW|RWR|RWS|Device'
 *
 * Put 'v' in to look at all the fields except for the followin
 * ./decode reglog.txt | grep -Ev 'RW|RWR|RWS|Device'
 *
 * To look at all the registers that differ from their defaults:
 * ./decode reglog.txt | grep -E 'diff|exam|Device'
 */


#include <stdio.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

enum reg_type {
	LH,
	LL,
	RES,
	RO,
	ROC,
	RW,
	RWC,
	RWR,
	RWS,
	SC,
	Update,
	Retain,
	WBAR,
	WO,
	RO_or_RW,
	NOTLISTED,
	RWR_or_RWS,
	RWS_except_port_bit,
	RWR_or_RO,
	RWS_to_ADDR,
	None,
	RWT,
	RWR_except,
};

static char *reg_type_str(enum reg_type type)
{
	switch (type) {
	case LH:
		return "LH";
	case LL:
		return "LL";
	case RES:
		return "RES";
	case RO:
		return "RO";
	case ROC:
		return "ROC";
	case RW: /* the only reg that has a distinct reset val */
		return "R/W";
	case RWC:
		return "RWC";
	case RWR:
		return "RWR";
	case RWS:
		return "RWS";
	case SC:
		return "SC";
	case Update:
		return "Update";
	case Retain:
		return "Retain";
	case WBAR:
		return "Retain";
	case WO:
		return "WO";
	case RO_or_RW:
		return "RO_or_RW";
	case NOTLISTED:
		return "NOTLISTED";
	case RWR_or_RWS:
		return "RWR_or_RWS";
	case RWS_except_port_bit:
		return "RWS_except_port_bit";
	case RWR_or_RO:
		return "RWS_except_port_bit";
	case RWS_to_ADDR:
		return "RWS_to_ADDR";
	case None:
		return "None";
	case RWT:
		return "RWT";
	case RWR_except:
		return "RWR_except";
	}
	return "No match";
}

struct field {
	char name[48];
	char bitnum;
	unsigned short mask;
	enum reg_type type;
	unsigned short reset_val;
};

void print_field(const struct field *f, unsigned short val)
{
	unsigned short field_val = (val >> f->bitnum) & f->mask;

	char init_val[16] = "";
	char diff_than_reset[16] = "";

	switch (f->type) {
	case RW:
	case RWS:
		sprintf(init_val, "%#6x", f->reset_val);
		if (field_val != f->reset_val)
			sprintf(diff_than_reset, "diff");
		break;
	case RWR:
		sprintf(init_val, "%#6x", 0);
		if (field_val != 0)
			sprintf(diff_than_reset, "diff");
		break;
	case RO_or_RW:
	case NOTLISTED:
	case RWR_or_RWS:
	case RWS_except_port_bit:
	case RWR_or_RO:
	case RWR_except:
		sprintf(diff_than_reset, "examine");
		break;
	default:
		break;
	}

	printf("%#6x %-20s %-6s %6s %6s\n",
	       field_val,
	       f->name,
	       reg_type_str(f->type),
	       init_val,
	       diff_than_reset);
}

void dec_port(unsigned char addr, unsigned short val)
{
	printf("Decode Reg %#x Val %#x\n", addr, val);
	switch (addr) {
	case 0x00:
		{
			printf("Port Status Register\n");
			const const struct field fields[] = {
				{"PauseEn", 15, 0x1, RO},
				{"MyPause", 14, 0x1, RO},
				{"Reserved", 13, 0x1, RES},
				{"PHYDetect", 12, 0x1, RWR},
				{"Link", 11, 0x1, RO},
				{"Duplex", 10, 0x1, RO},
				{"Speed", 8, 0x3, RO},
				{"Reserved", 7, 0x1, RES},
				{"EEE Enabled", 6, 0x1, RO},
				{"TxPaused", 5, 0x1, RO},
				{"FlowCtrl", 4, 0x1, RO},
				{"C_Mode", 0, 0xF, RO_or_RW},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x01:
		{
			printf("Physical Control Register\n");
			const struct field fields[] = {
				{"RGMII Rx Timing", 15, 0x1, RWR},
				{"RGMII Tx Timing", 14, 0x1, RWR},
				{"Reserved", 13, 0x1, RES},
				{"200BASE", 12, 0x1, RWR},
				{"MII PHY", 11, 0x1, RWR},
				{"Reserved", 8, 0x7, RES},
				{"FCValue", 7, 0x3, RWR},
				{"ForcedFC", 6, 0x1, NOTLISTED},
				{"LinkValue", 5, 0x1, RWR},
				{"ForcedLink", 4, 0x1, RWR},
				{"DpxValue", 3, 0x1, RWR},
				{"ForcedDpx", 2, 0x1, RWR},
				{"ForceSpd", 0, 0x3, RWS, 0x3},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x02:
		{
			printf("Jamming Control Register\n");
			const struct field fields[] = {
				{"LimitOut", 8, 0xFF, RWS, 0xFF},
				{"LimitIn", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x03:
		{
			printf("Switch Identifier Register\n");
			const struct field fields[] = {
				{"Product Num", 4, 0xFFF, RO},
				{"Rev", 0, 0xF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x04:
		{
			printf("Port Control Register\n");
			const struct field fields[] = {
				{"SA Filtering", 14, 0x3, RWR},
				{"Egress Mode", 12, 0x3, RWR},
				{"Header", 11, 0x1, RWR},
				{"IGMP/MLD Snoop", 10, 0x1, RWR},
				{"Frame Mode", 8, 0x3, RWR},
				{"VLAN Tunnel", 7, 0x1, RWR},
				{"TagIfBoth", 6, 0x1, RWS, 0x1},
				{"InitialPri", 4, 0x3, RWS, 0x3},
				{"Egress Floods", 2, 0x3, RWS, 0x3},
				{"PortState", 0, 0x3, RWR_or_RWS},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x05:
		{
			printf("Port Control 1\n");
			const struct field fields[] = {
				{"Message Port", 15, 0x1, RWR},
				{"Trunk Port", 14, 0x1, RWR},
				{"Reserved", 12, 0x3, RES},
				{"Trunk ID", 8, 0xF, RWR},
				{"FID [11:4]", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x06:
		{
			printf("Port Based VLAN Map\n");
			const struct field fields[] = {
				{"FID [3:0]", 12, 0xF, RWR},
				{"ForceMap", 11, 0x1, RWR},
				{"Reserved", 7, 0xF, RES},
				{"VLANTable", 0, 0x7F, RWS_except_port_bit},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x07:
		{
			printf("Default Port VLAN ID & Priority\n");
			const struct field fields[] = {
				{"DefPri", 13, 0x7, RWR},
				{"Force DefaultVID", 12, 0x1, RWR},
				{"DefaultVID", 0, 0xFFF, 0x001},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x08:
		{
			printf("Port Control 2 Register\n");
			const struct field fields[] = {
				{"ForceGoodFCS", 15, 0x1, RWR},
				{"AllowBadFCS", 14, 0x1, RWR},
				{"Jumbo Mode", 12, 0x3, RWS, 0x2},
				{"802.1QMode", 10, 0x3, RWR},
				{"Discard Untagged", 8, 0x1, RWR},
				{"MapDA", 7, 0x1, RWS, 0x1},
				{"ARP Mirror", 6, 0x1, RWR},
				{"Egress Monitor Source", 5, 0x1, RWR},
				{"Ingress Monitor Source", 4, 0x1, RWR},
				{"Use Def Qpri", 3, 0x1, RWR},
				{"DefQpri", 1, 0x3, RWR},
				{"Reserved", 0, 0x1, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x09:
		{
			printf("Egress Rate Control\n");
			const struct field fields[] = {
				{"Reserved", 12, 0xF, RES},
				{"Frame Overhead", 8, 0xF, RWR},
				{"Reserved", 7, 0x1, RES},
				{"Egress Dec", 0, 0x3F, RWS, 0x1},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0A:
		{
			printf("Egress Rate Control 2\n");
			const struct field fields[] = {
				{"Count Mode", 14, 0x3, RWS, 0x2},
				{"Schedule", 12, 0x3, RWR},
				{"Egress Rate", 0, 0xFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0B:
		{
			printf("Port Association Vector\n");
			const struct field fields[] = {
				{"HoldAt1", 15, 0x1, RWR},
				{"IntOn AgeOut", 14, 0x1, RWR},
				{"LockedPort", 13, 0x1, RWR},
				{"Ignore WrongData", 12, 0x1, RWR},
				{"Refresh Locked", 11, 0x1, RWR},
				{"Reserved", 7, 0xF, RES},
				{"PAV", 0, 0xFF, RWS_except_port_bit},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0C:
		{
			printf("Port ATU Control\n");
			const struct field fields[] = {
				{"Read LearnCnt", 15, 0x1, RWR},
				{"Limit Reached", 14, 0x1, RO},
				{"OverLimit IntEn", 13, 0x1, RWR},
				{"KeepOldLearnLimit", 12, 0x1, RWR},
				{"Reserved", 10, 0x3, RES},
				{"LearnLimit/LearnCnt", 0, 0x3FF, RWR_or_RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0D:
		{
			printf("Priority Override Register\n");
			const struct field fields[] = {
				{"DAPri Override", 14, 0x3, RWR},
				{"SAPri Override", 12, 0x3, RWR},
				{"VTUPri Override", 10, 0x3, RWR},
				{"Mirror SA Miss", 9, 0x1, RWR},
				{"Mirror VTU Miss", 8, 0x1, RWR},
				{"Trap DA Miss", 7, 0x1, RWR},
				{"Trap SA Miss", 6, 0x1, RWR},
				{"Trap VTU Miss", 5, 0x1, RWR},
				{"Trap TCAM Miss (88E6321 only)", 4, 0x1, RWR},
				{"Reserved", 2, 0x3, RES},
				{"TCAM Mode (88E6321 only)", 0, 0x3, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0E:
		{
			printf("Policy Control Register\n");
			const struct field fields[] = {
				{"DA Policy", 14, 0x3, RWR},
				{"SA Policy", 12, 0x3, RWR},
				{"VTU Policy", 10, 0x3, RWR},
				{"EType Policy", 8, 0x3, RWR},
				{"PPPoE Policy", 6, 0x3, RWR},
				{"VBAS Policy", 4, 0x3, RWR},

				{"Opt82 Policy", 2, 0x3, RWR},
				{"UDP Policy", 0, 0x3, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0F:
		{
			printf("Port E Type\n");
			const struct field fields[] = {
				{"Port EType", 0, 0xFFFF, RWS, 0x9100},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
		{
			printf("Reserved\n");
			const struct field fields[] = {
				{"Reserved", 0, 0xFFFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x16:
		{
			printf("LED Control\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Pointer", 12, 0x7, RWR},
				{"Reserved", 11, 0x1, RES},
				{"Data", 0, 0x7FF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x18:
		{
			printf("Port IEEE Priority Remapping Registers\n");
			const struct field fields[] = {
				{"Reserved", 15, 0x1, RES},
				{"TagRemap3", 12, 0x7, RWS, 0x3},
				{"Reserved", 11, 0x1, RES},
				{"TagRemap2", 8, 0x7, RWS, 0x2},
				{"Reserved", 7, 0x1, RES},
				{"TagRemap1", 4, 0x7, RWS, 0x1},
				{"Reserved", 3, 0x1, RES},
				{"TagRemap0", 0, 0x7, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x19:
		{
			printf("Port IEEE Priority Remapping Registers\n");
			const struct field fields[] = {
				{"Reserved", 15, 0x1, RES},
				{"TagRemap7", 12, 0x7, RWS, 0x7},
				{"Reserved", 11, 0x1, RES},
				{"TagRemap6", 8, 0x7, RWS, 0x6},
				{"Reserved", 7, 0x1, RES},
				{"TagRemap5", 4, 0x7, RWS, 0x5},
				{"Reserved", 3, 0x1, RES},
				{"TagRemap4", 0, 0x7, RWS, 0x4},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1A:
		{
			printf("Reserved\n");
			const struct field fields[] = {
				{"Reserved", 0, 0xFFFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1B:
		{
			printf("Queue Counter Registers\n");
			const struct field fields[] = {
				{"Mode", 12, 0xF, RWS, 0x8},
				{"Self Inc", 11, 0x1, RWR},
				{"Reserved", 9, 0x3, RES},
				{"Data", 0, 0x1FF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1C:
	case 0x1D:
		{
			printf("Reserved\n");
			const struct field fields[] = {
				{"Reserved", 0, 0xFFFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1E:
		{
			printf("Debug Counter\n");
			const struct field fields[] = {
				{"RxBad Frames/Tx Collisions", 8, 0xFF, RO},
				{"RxGood Frames/Tx Transmit Frames", 0, 0xFF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1F:
		{
			printf("Cut Through Register\n");
			const struct field fields[] = {
				{"Enable Select (valid on Port 2, 5, and 6 only)", 12, 0xF, RWS, 0xF},
				{"Reserved", 9, 0x7, RES},
				{"Cut Through (88E6321 only)", 8, 0x1, RWR},
				{"Reserved", 4, 0xF, RES},
				{"Cut Through Queue (88E6321 only)", 0, 0xF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	}
}

void dec_gbl1(unsigned char addr, unsigned short val)
{
	printf("Decode Reg %#x Val %#x\n", addr, val);
	switch (addr) {
	case 0x00:
		{
			printf("Switch Global Status Register\n");
			const struct field fields[] = {
				{"PPUState", 15, 0x1, RO},
				{"Reserved", 12, 0x3, RES},
				{"InitReady", 11, 0x1, RO},
				{"Reserved", 9, 0x3, RES},
				{"AVBInt", 8, 0x1, RO},
				{"DeviceInt", 7, 0x1, RO},
				{"StatsDone", 6, 0x1, LH},
				{"VTUProb", 5, 0x1, RO},
				{"VTUDone", 4, 0x1, LH},
				{"ATUProb", 3, 0x1, RO},
				{"ATUDone", 2, 0x1, LH},
				{"TCAM Int (88E6321 only)", 1, 0x1, ROC},
				{"EEInt", 0, 0x1, LH},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x01:
		{
			printf("ATU FID Register\n");
			const struct field fields[] = {
				{"Reserved", 12, 0xF, RES},
				{"FID", 0, 0xFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x02:
		{
			printf("VTU FID Register\n");
			const struct field fields[] = {
				{"Reserved", 13, 0x7, RES},
				{"VIDPolicy", 12, 0x1, RWR},
				{"FID", 0, 0xFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x03:
		{
			printf("VTU SID Register\n");
			const struct field fields[] = {
				{"Reserved", 6, 0x3FF, RES},
				{"SID", 0, 0x3F, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x04:
		{
			printf("Switch Global Control Register\n");
			const struct field fields[] = {
				{"SWReset", 15, 0x1, SC},
				{"Reserved", 14, 0x1, RWS, 0x1},
				{"Discard Excessive", 13, 0x1, RWR},
				{"ARPwo BC", 12, 0x1, RWR},
				{"Reserved", 10, 0x3, RES},
				{"Reserved", 9, 0x1, RES},
				{"AVBIntEn", 8, 0x1, RO},
				{"DevIntEn", 7, 0x1, RWR},
				{"StatsDone IntEn", 6, 0x1, RWR},
				{"VTUProb IntEn", 5, 0x1, RWR},
				{"VTUDone IntEn", 4, 0x1, RWR},
				{"ATUProb IntEn", 3, 0x1, RWR},
				{"ATUDone IntEn", 2, 0x1, RWR},
				{"TCAM IntEn (88E6321 only)", 1, 0x1, RWR},
				{"EEIntEn", 0, 0x1, RWS, 0x1},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x05:
		{
			printf("VTU Operation Register\n");
			const struct field fields[] = {
				{"VTWBusy", 15, 0x1, SC},
				{"VTUOp", 12, 0x7, RWR},
				{"Reserved", 7, 0x1F, RES},
				{"Member Violation", 6, 0x1, RO},
				{"Miss Violation", 5, 0x1, RO},
				{"Reserved", 4, 0x1, RES},
				{"SPID", 0, 0xF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x06:
		{
			printf("VTU VID Register\n");
			const struct field fields[] = {
				{"Reserved", 13, 0x7, RES},
				{"Valid", 12, 0x1, RWR},
				{"VID", 0, 0xFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x07:
		{
			printf("VTU/STU Data Register Ports 0 to 3 for VTU Operations\n");
			const struct field fields[] = {
				{"Member TagP3", 12, 0x3, RWR},
				{"Reserved", 10, 0x3, RES},
				{"Member TagP2", 8, 0x3, RWR},
				{"Reserved", 6, 0x3, RES},
				{"Member TagP1", 4, 0x3, RWR},
				{"Reserved", 2, 0x3, RES},
				{"Member TagP0", 0, 0x3, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			printf("VTU/STU Data Register Ports 0 to 3 for STU Operations\n");
			const struct field fields2[] = {
				{"PortState P3", 14, 0x3, RWR},
				{"Reserved", 12, 0x3, RES},
				{"PortState P2", 10, 0x3, RWR},
				{"Reserved", 8, 0x3, RES},
				{"PortState P1", 6, 0x3, RWR},
				{"Reserved", 4, 0x3, RES},
				{"PortState P0", 2, 0x3, RWR},
				{"Reserved", 0, 0x3, RES},
			};
			int i2;

			for (i2 = 0; i2 < ARRAY_SIZE(fields2); i2++)
				print_field(&fields2[i2], val);

			break;
		}
	case 0x08:
		{
			printf("VTU/STU Data Register Ports 4 to 5 for VTU Operations\n");
			const struct field fields[] = {
				{"Reserved", 10, 0x3F, RES},
				{"Member TagP6", 8, 0x3, RWR},
				{"Reserved", 6, 0x3, RES},
				{"Member TagP5", 4, 0x3, RWR},
				{"Reserved", 2, 0x3, RES},
				{"Member TagP4", 0, 0x3, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			printf("VTU/STU Data Register Ports 4 to 5 for STU Operations\n");
			const struct field fields2[] = {
				{"PortState P6", 10, 0x3, RWR},
				{"Reserved", 8, 0x3, RES},
				{"PortState P5", 6, 0x3, RWR},
				{"Reserved", 4, 0x3, RES},
				{"PortState P4", 2, 0x3, RWR},
				{"Reserved", 0, 0x3, RES},
			};
			int i2;

			for (i2 = 0; i2 < ARRAY_SIZE(fields2); i2++)
				print_field(&fields2[i2], val);

			break;
		}
	case 0x09:
		{
			printf("VTU/STU Data Register for VTU Operations\n");
			const struct field fields[] = {
				{"VIDPRI", 12, 0x7, RWR},
				{"Reserved", 0, 0xFFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0A:
		{
			printf("ATU Control Register\n");
			const struct field fields[] = {
				{"MACAVB", 15, 0x1, RWR},
				{"Reserved", 12, 0x7, RES},
				{"AgeTime", 4, 0xFF, RWS, 0x16},
				{"Learn2All", 3, 0x1, RWR},
				{"Reserved", 2, 0x1, RES},
				{"HashSel", 0, 0x3, RWS, 0x1},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0B:
		{
			printf("ATU Operation Register\n");
			const struct field fields[] = {
				{"ATUBusy", 15, 0x1, SC},
				{"ATUOp", 12, 0x7, RWR},
				{"Reserved", 11, 0x1, RES},
				{"MACPri", 8, 0x7, RWR},
				{"AgeOut Violation", 7, 0x1, RO},
				{"Member Violation", 6, 0x1, RO},
				{"Miss Violation", 5, 0x1, RO},
				{"ATUFull Violation", 4, 0x1, RO},
				{"Reserved", 0, 0xF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0C:
		{
			printf("ATU Data Register\n");
			const struct field fields[] = {
				{"Trunk", 15, 0x1, RWR},
				{"Reserved", 12, 0x7, RES},
				{"PortVec/ToPort & FromPort", 4, 0xFF, RWR},
				{"EntryState/SPID", 0, 0xF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0D:
		{
			printf("ATU MAC Address Register Bytes 0 & 1\n");
			const struct field fields[] = {
				{"ATUByte0", 8, 0xFF, RWR},
				{"ATUByte1", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0E:
		{
			printf("ATU MAC Address Register Bytes 2 & 3\n");
			const struct field fields[] = {
				{"ATUByte2", 8, 0xFF, RWR},
				{"ATUByte3", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0F:
		{
			printf("ATU MAC Address Register Bytes 4 & 5\n");
			const struct field fields[] = {
				{"ATUByte4", 8, 0xFF, RWR},
				{"ATUByte5", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
		{
			printf("Reserved\n");
			const struct field fields[] = {
				{"Reserved", 0, 0xFFFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x18:
		{
			printf("IEEE-PRI Register\n");
			const struct field fields[] = {
				{"Tag_0x7", 14, 0x3, RWS, 0x3},
				{"Tag_0x6", 12, 0x3, RWS, 0x3},
				{"Tag_0x5", 10, 0x3, RWS, 0x3},
				{"Tag_0x4", 8, 0x3, RWS, 0x3},
				{"Tag_0x3", 6, 0x3, RWS, 0x3},
				{"Tag_0x2", 4, 0x3, RWS, 0x3},
				{"Tag_0x1", 2, 0x3, RWR},
				{"Tag_0x0", 0, 0x3, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x19:
		{
			printf("IP Mapping Table\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"UseIPFPri", 14, 0x1, RWR},
				{"Pointer", 8, 0x3F, RWR},
				{"Reserved", 7, 0x1, RES},
				{"IP_FPRI", 4, 0x7, RWS},
				{"Reserved", 2, 0x7, RES},
				{"IP_QPRI", 0, 0x3, RWS},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1A:
		{
			printf("Monitor Control\n");
			const struct field fields[] = {
				{"Ingress Monitor Dest", 12, 0xF, RWS, 0xF},
				{"Egress Monitor Dest", 8, 0xF, RWS, 0xF},
				{"CPU Dest", 4, 0xF, RWS, 0xF},
				{"Mirror Dest", 4, 0xF, RWS, 0xF},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1B:
		{
			printf("Total Free Counter\n");
			const struct field fields[] = {
				{"Reserved", 10, 0x3F, RES},
				{"FreeQSize", 0, 0x3FF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1C:
		{
			printf("Global Control 2\n");
			const struct field fields[] = {
				{"Header Type", 14, 0x3, RWR},
				{"RMU Mode", 12, 0x3, RWR},
				{"DA Check", 11, 0x1, RWR},
				{"Reserved", 6, 0x1F, RES},
				{"CtrMode", 5, 0x1, RWR},
				{"DeviceNumber", 0, 0x1F, RWS_to_ADDR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1D:
		{
			printf("Stats Operation Register\n");
			const struct field fields[] = {
				{"StatsBusy", 15, 0x1, SC},
				{"StatsOp", 12, 0x7, RWR},
				{"Histogram Mode", 10, 0x3, RWS, 0x3},
				{"StatsBank", 9, 0x1, RWR},
				{"StatsPort", 5, 0xF, RWR},
				{"StatsPtr", 0, 0x1F, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1E:
		{
			printf("Stats Counter Register Bytes 3 & 2\n");
			const struct field fields[] = {
				{"StatsByte3", 8, 0xFF, RO},
				{"StatsByte2", 8, 0xFF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1F:
		{
			printf("Stats Counter Register Bytes 1 & 0\n");
			const struct field fields[] = {
				{"StatsByte1", 8, 0xFF, RO},
				{"StatsByte0", 8, 0xFF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	}
}

void dec_gbl2(unsigned char addr, unsigned short val)
{
	printf("Decode Reg %#x Val %#x\n", addr, val);
	switch (addr) {
	case 0x00:
		{
			printf("Interrupt Source Register\n");
			const struct field fields[] = {
				{"WatchDog Int", 15, 0x1, RO},
				{"JamLimit", 14, 0x1, ROC},
				{"Duplex Mismatch", 13, 0x1, ROC},
				{"WakeEvent", 12, 0x1, RO},
				{"Reserved", 5, 0x7F, RES},
				{"PHYInt", 3, 0x3, RO},
				{"Reserved", 2, 0x1, RES},
				{"SERDES Int", 0, 0x3, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x01:
		{
			printf("Interrupt Mask Register\n");
			const struct field fields[] = {
				{"WatchDog IntEn", 15, 0x1, RWR},
				{"JamLimitEn", 14, 0x1, ROC},
				{"Duplex Mismatch Error", 13, 0x1, RWR},
				{"WakeEventEn", 12, 0x1, RWR},
				{"Reserved", 5, 0x7F, RES},
				{"PHYIntEn", 3, 0x3, RWR},
				{"Reserved", 2, 0x1, RES},
				{"SERDES IntEn", 0, 0x3, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x02:
		{
			printf("MGMT Enable Register 2x\n");
			const struct field fields[] = {
				{"Rsvd2CPU Enables 2x", 0, 0xFFFF, RWS, 0xFFFF},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x03:
		{
			printf("MGMT Enable Register 0x\n");
			const struct field fields[] = {
				{"Rsvd2CPU Enables 0x", 0, 0xFFFF, RWS, 0xFFFF},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x04:
		{
			printf("Flow Control Delay Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"SPD", 13, 0x3, RWR},
				{"FC Delay Time", 0, 0x1FFF, RWS, 0x258},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x05:
		{
			printf("Switch Management Register\n");
			const struct field fields[] = {
				{"Loopback Filter", 15, 0x1, RWR},
				{"Reserved", 14, 0x1, RES},
				{"Flow Control Message", 13, 0x1, RWR_or_RWS},
				{"FloodBC", 12, 0x1, RWR},
				{"Remove 1PTag", 11, 0x1, RWR},
				{"ATUAge IntEn", 10, 0x1, RWS, 0x1},
				{"Tag Flow Control", 9, 0x1, RWR},
				{"Reserved", 8, 0x1, RES},
				{"ForceFlow ControlPri", 7, 0x1, RWS, 0x1},
				{"FC Pri", 4, 0x7, RWS, 0x7},
				{"Rsvd2CPU", 3, 0x7, None},
				{"MGMT Pri", 0, 0x7, RWS, 0x7},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x06:
		{
			printf("Device Mapping Table Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Reserved", 13, 0x3, RES},
				{"Trg_Dev Value", 8, 0x1F, RWR},
				{"Reserved", 4, 0xF, RES},
				{"Trg_Dev Port", 0, 0xF, RWS, 0xF},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x07:
		{
			printf("Trunk Mask Table Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"MaskNum", 12, 0x7, RWR},
				{"HashTrunk", 11, 0x1, RWR},
				{"Reserved", 7, 0xF, RES},
				{"TrunkMask", 0, 0x7F, RWS, 0x7F},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x08:
		{
			printf("Trunk Mapping Table Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Trunk ID", 11, 0xF, RWR},
				{"Reserved", 7, 0xF, RES},
				{"TrunkMap", 0, 0x7F, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x09:
		{
			printf("Ingress Rate Command Register\n");
			const struct field fields[] = {
				{"IRLBusy", 15, 0x1, SC},
				{"IRLOp", 12, 0x7, RWR},
				{"IRLPort", 8, 0xF, RES},
				{"IRLRes", 5, 0x7, RWR},
				{"Reserved", 4, 0x1, RWR},
				{"IRLReg", 0, 0xF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0A:
		{
			printf("Ingress Rate Data Register\n");
			const struct field fields[] = {
				{"IRLData", 0, 0xFFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0B:
		{
			printf("Cross-chip Port VLAN Register\n");
			const struct field fields[] = {
				{"PVTBusy", 15, 0x1, SC},
				{"PVTOp", 12, 0x7, RWR},
				{"Reserved", 9, 0x7, RES},
				{"Pointer", 0, 0x1FF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0C:
		{
			printf("Cross-chip Port VLAN Data Register\n");
			const struct field fields[] = {
				{"Reserved", 7, 0x1FF, SC},
				{"PVLAN Data", 0, 0x7F, RWS, 0x7F},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0D:
		{
			printf("Switch MAC/WoL/WoF Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Reserved", 13, 0x3, RES},
				{"Pointer", 8, 0x1F, RWR},
				{"Data", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0E:
		{
			printf("ATU Stats Register\n");
			const struct field fields[] = {
				{"Bin", 14, 0x3, RWR},
				{"CountMode", 12, 0x3, RWR},
				{"ActiveBin Ctr", 0, 0xFFF, RO},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x0F:
		{
			printf("Priority Override Table\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Reserved", 13, 0x3, RES},
				{"FPriSet", 12, 0x1, RWR},
				{"Pointer", 8, 0xF, RWT},
				{"QpriAvbEn", 7, 0x1, RWR},
				{"Reserved", 6, 0x1, RES},
				{"DataAvb", 4, 0x3, RWR},
				{"QPriEn or FPriEn", 3, 0x1, RWR_except},
				{"Data", 0, 0x7, RWR_except},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		{
			printf("Reserved\n");
			const struct field fields[] = {
				{"Reserved", 0, 0xFFFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x14:
		{
			printf("EEPROM Command\n");
			const struct field fields[] = {
				{"EEBusy", 15, 0x1, SC},
				{"EEOp", 12, 0x7, RWR},
				{"Running", 11, 0x1, RO},
				{"WriteEn", 10, 0x1, RO},
				{"Reserved", 8, 0x3, RO},
				{"Addr", 0, 0xFF, RES},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x15:
		{
			printf("EEPROM Data\n");
			const struct field fields[] = {
				{"Data", 0, 0xFFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x16:
		{
			printf("AVB Command Register\n");
			const struct field fields[] = {
				{"AVBBusy", 15, 0x1, SC},
				{"AVBOp", 12, 0x7, RWR},
				{"AVBPort", 8, 0xF, RWR},
				{"AVBBLock", 5, 0x7, RWR},
				{"AVBAddr", 0, 0xF, None},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x17:
		{
			printf("AVB Data Register\n");
			const struct field fields[] = {
				{"AVBData", 0, 0xFFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x18:
		{
			printf("SMI PHY Command Register\n");
			const struct field fields[] = {
				{"SMIBusy", 15, 0x1, SC},
				{"Reserved", 13, 0x3, RES},
				{"SMIMode", 12, 0x1, RWR},
				{"SMIOp", 10, 0x3, RWR},
				{"DevAddr", 5, 0x1F, RWR},
				{"RegAddr", 0, 0x1F, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x19:
		{
			printf("SMI PHY Data Register\n");
			const struct field fields[] = {
				{"SMIData", 0, 0xFFFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1A:
		{
			printf("Scratch and Misc. Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Pointer", 8, 0x7F, RWR},
				{"Data", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	case 0x1B:
		{
			printf("Watch Dog Control Register\n");
			const struct field fields[] = {
				{"Update", 15, 0x1, SC},
				{"Pointer", 8, 0x7F, RWR},
				{"Data", 0, 0xFF, RWR},
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(fields); i++)
				print_field(&fields[i], val);

			break;
		}
	}
}

void dec_gbl3(unsigned char addr, unsigned short val)
{
	printf("%s(%#x, %#x)\n", __func__, addr, val);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[255];

	if (argc == 1) {
		perror("Please pass a text file to decode\n");
		return(-1);
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("Error opening file\n");
		return(-1);
	}

	int device = 0;

	while (fgets(buf, 255, (FILE *) fp) != NULL) {
		printf("%s", buf);
		int ret;

		{
			ret = sscanf(buf, "Device %x Registers:", &device);
			if (ret == 1)
				continue;
		}
		{
			int addr, val;

			ret = sscanf(buf, "%x -> %x", &addr, &val);
			if (ret == 2) {
				switch (device) {
				case 0x10:
				case 0x11:
				case 0x12:
				case 0x13:
				case 0x14:
				case 0x15:
				case 0x16:
					printf("Port %#x\n", device);
					dec_port(addr, val);
					break;
				case 0x1b:
					dec_gbl1(addr, val);
					break;
				case 0x1c:
					dec_gbl2(addr, val);
					break;
				case 0x1d:
					dec_gbl3(addr, val);
					break;
				}
				continue;
			}
		}
	}

	fclose(fp);

	return 0;
}
