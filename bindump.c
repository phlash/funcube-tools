#include <stdio.h>
#include <stdint.h>

uint32_t getbits(uint8_t *pkt, int off, int len) {
	uint32_t rv = 0;
	int idx;
	int bit;
	while (len--) {
		rv<<=1;
		idx=off/8;
		bit=7-(off%8);
		rv|=(pkt[idx] & (1<<bit)) ? 1 : 0;
		++off;
	}
	return rv;
}
 
static char *csv_fc1="ID,FRAME,PV1,PV2,PV3,PCUR,VBAT,SCUR,RCNT,EPSE,B1T,B2T,B3T,TBAT,5V1L,3V3L,RESC,PPTM,"
	"SUNX,SUNY,SUNZ,SPX+T,SPX-T,SPY+T,SPY-T,3V3V,3V3C,5V0V,RXDP,RSSI,RFT,RXC,TXC3,TXC5,PAFW,PARV,PAT,PAC"
	"ANT0,ANT1,AND0,AND1,AND2,AND3,SEQ,CCNT,LCMD,CMDOK,DVAS,DVEPS,DVPA,DVRF,DVMSE,DVAN1,DVAN0,ECLP,SAFE,HABF,SABF,WAIT\n";
static char *csv_ukube="ID,FRAME,SUN,SCUR0,SCUR1,SCUR2,SCUR3,SCUR4,SCUR5,SCUR6,SCUR7,SCUR8,SCUR9,SCUR10,SCUR11,STEMP,"
	"B0D,B0C,B0CV,B0V,B0T,B1D,B1C,B1CV,B1V,B1T,B2D,B2C,B2CV,B2V,B2T,BHEAT,ATIME,A0S,A1S,A2S,A3S,ATEMP,"
	"RF0,RF1,RF2,RF3,RF4,RF5,PA0,PA1,PA2,PA3,MAG0,MAG1,MAG2,MTEMP,SEQ,CCNT,LCMD,CMDOK\n";
static char *csv_nayif="";

void csvprint(uint32_t v, FILE *cp) {
	if (cp) {
		fprintf(cp, "%u,", v);
	}
}

void decode_fc1(uint8_t *pkt, FILE *cp) {
	uint32_t v;
	int i;
	v = getbits(pkt, 0, 2);
	printf("ID: %x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 2, 6);
	printf("frame: %02x ", v);
	csvprint(v, cp);
	for (i=0; i<3; i++) {
		v = getbits(pkt, 8+(16*i), 16);
		printf("pV%i: %04x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 56, 16);
	printf("pA: %04x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 72, 16);
	printf("batt V: %04x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 88, 16);
	printf("sys A: %04x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 104, 16);
	printf("reboots: %04x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 120, 16);
	printf("EPS errs: %04x ", v);
	csvprint(v, cp);
	for (i=0; i<3; i++) {
		v = getbits(pkt, 136+(8*i), 8);
		printf("BC%i temp: %02x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 160, 8);
	printf("batt T: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 168, 8);
	printf("latch 5v0: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 176, 8);
	printf("latch 3v3: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 184, 8);
	printf("EPS cause: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 192, 8);
	printf("PPT mode: %02x ", v);
	csvprint(v, cp);
	for (i=0; i<3; i++) {
		v = getbits(pkt, 200+(10*i), 10);
		printf("sun%d: %03x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 230, 10);
	printf("panel T+X: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 240, 10);
	printf("panel T-X: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 250, 10);
	printf("panel T+Y: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 260, 10);
	printf("panel T-Y: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 270, 10);
	printf("3v3 V: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 280, 10);
	printf("3v3 A: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 290, 10);
	printf("5v0 V: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 300, 8);
	printf("RX doppler: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 308, 8);
	printf("RSSI: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 316, 8);
	printf("RX Temp: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 324, 8);
	printf("RX A: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 332, 8);
	printf("TX3v3 A: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 340, 8);
	printf("TX5v0 A: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 348, 8);
	printf("PA Fwd: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 356, 8);
	printf("PA Rev: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 364, 8);
	printf("PA Temp: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 372, 8);
	printf("PA A: %02x ", v);
	csvprint(v, cp);
	for (i=0; i<2; i++) {
		v = getbits(pkt, 380+(8*i), 8);
		printf("Ants%d T: %02x ", i, v);
		csvprint(v, cp);
	}
	for (i=0; i<4; i++) {
		v = getbits(pkt, 396+i, 1);
		printf("Ants%d D: %x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 400, 24);
	printf("Seq: %06x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 424, 6);
	printf("DTMF cnt: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 430, 5);
	printf("DTMF cmd: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 435, 1);
	printf("DTMF ok: %x ", v);
	csvprint(v, cp);
	for (i=0; i<7; i++) {
		v = getbits(pkt, 436+i, 1);
		printf("DV%d: %x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 443, 1);
	printf("Eclipse?: %x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 444, 1);
	printf("Safe?: %x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 445, 1);
	printf("hw abf?: %x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 446, 1);
	printf("sw abf?: %x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 447, 1);
	printf("Dep wait?: %x\n\n", v);
	csvprint(v, cp);
}

void decode_ukube(uint8_t *pkt, FILE *cp) {
	uint32_t v;
	int i;
	v = getbits(pkt, 0, 2);
	printf("ID: %x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 2, 6);
	printf("frame: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 8, 1);
	printf("sunlight: %x\n", v);
	csvprint(v, cp);
	printf("solar current: ");
	for (i=0; i<12; i++) {
		v = getbits(pkt, 9+(10*i), 10);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 129, 8);
	printf("solar temp: %02x\n", v);
	csvprint(v, cp);
	for (i=0; i<3; i++) {
		v = getbits(pkt, 137+(33*i), 1);
		printf("batt[%d] dir: %x ", i, v);
		csvprint(v, cp);
		v = getbits(pkt, 138+(33*i), 8);
		printf("current: %02x ",  v);
		csvprint(v, cp);
		v = getbits(pkt, 146+(33*i), 8);
		printf("cell volts: %02x ",  v);
		csvprint(v, cp);
		v = getbits(pkt, 154+(33*i), 8);
		printf("voltage: %02x ",  v);
		csvprint(v, cp);
		v = getbits(pkt, 162+(33*i), 8);
		printf("temp: %02x\n",  v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 236, 1);
	printf("batt heater: %x\n", v);
	csvprint(v, cp);
	v = getbits(pkt, 237, 8);
	printf("ants timeout: %02x ", v);
	csvprint(v, cp);
	for (i=0; i<4; i++) {
		v = getbits(pkt, 245+(3*i), 3);
		printf("ants[%d] status: %x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 257, 8);
	printf("ants temp: %02x\n", v);
	csvprint(v, cp);
	for (i=0; i<6; i++) {
		v = getbits(pkt, 265+(8*i), 8);
		printf("rf data[%d]: %02x ", i, v);
		csvprint(v, cp);
	}
	printf("\n");
	for (i=0; i<4; i++) {
		v = getbits(pkt, 313+(8*i), 8);
		printf("pa data[%d]: %02x ", i, v);
		csvprint(v, cp);
	}
	printf("\n");
	for (i=0; i<3; i++) {
		v = getbits(pkt, 345+(16*i), 16);
		printf("magnetometer[%d]: %04x ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 393, 10);
	printf("magnetometer temp: %x\n", v);
	csvprint(v, cp);
	v = getbits(pkt, 403, 9);
	printf("padding: %x\n", v);
	v = getbits(pkt, 412, 24);
	printf("sequence: %03x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 436, 6);
	printf("dtmf cmd count: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 442, 5);
	printf("dtmf last cmd: %02x ", v);
	csvprint(v, cp);
	v = getbits(pkt, 447, 1);
	printf("dtmf cmd success: %x\n\n", v);
	if (cp)
		fprintf(cp, "%u\n", v);
}

void decode_nayif(uint8_t *pkt, FILE *cp) {
}

void dump(uint8_t *pkt) {
	int i;
	for (i=0; i<56; i++)
		printf("%02x ", pkt[i]);
	printf("\n");
}

typedef void (*decode_t)(uint8_t *, FILE *);

int usage() {
	puts("usage: bindump [-u[kube]] [-n[ayif]] [-f <bin file>] [-c <csv output file>]");
	return 0;
}

int main(int argc, char **argv) {
	char *bin = "tlmtestcapture.funcubebin";
	char *csv = NULL, *csv_hdrs = csv_fc1;
	FILE *fp, *cp = NULL;
	decode_t pdec = decode_fc1;
	uint8_t pkt[256];
	int arg;

	for (arg=1; arg<argc; arg++) {
		if (!strncmp(argv[arg], "-f", 2))
			bin = argv[++arg];
		else if (!strncmp(argv[arg], "-u", 2)) {
			pdec = decode_ukube;
			csv_hdrs = csv_ukube;
		} else if (!strncmp(argv[arg], "-n", 2)) {
			pdec = decode_nayif;
			csv_hdrs = csv_nayif;
		} else if (!strncmp(argv[arg], "-c", 2))
			csv = argv[++arg];
		else
			return usage();
	}
	fprintf(stderr, "dumping: %s\n", bin);
	fp = fopen(bin, "rb");
	if (!fp) {
		perror("opening bin file");
		return 1;
	}
	if (csv) {
		cp = fopen(csv, "wb");
		if (!cp) {
			perror("opening CSV file");
			fclose(fp);
			return 1;
		} else {
			fputs(csv_hdrs, cp);
		}
	}
	while (fread(pkt, sizeof(pkt), 1, fp)==1) {
		dump(pkt);
		pdec(pkt, cp);
	}
	fclose(fp);
	if (cp)
		fclose(cp);
	return 0;
}
