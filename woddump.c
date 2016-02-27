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
 
static char *csv_fc1="CNT,MSE0,MSE1,MSE2,MSE3,ASIB0,ASIB1,ASIB2,ASIB3,EPS0,EPS1,EPS2,EPS3,EPS4,EPS5\n";

static char *csv_ukube="CNT,MSE0,MSE1,MSE2,MSE3,AMAC0,AMAC1,AMAC2,AMAC3,AMAC4,"
	"BCUR0,BCEL0,BVLT0,BTMP0,BDIR1,BCUR1,BCEL1,BVLT1,BTMP1,BDIR2,BCUR2,BCEL2,BVLT2,PAD\n";

static char *csv_nayif="CNT,TMPC,TMPR,TMPP,ASIB0,ASIB1,ASIB2,ASIB3,ASIB4,ASIB5,ASIB6,ASIB7,ASIB8,ASIB9,ASIB10,ASIB11,TMPB,PCUR,BVLT,SCUR\n";

void csvprint(uint32_t v, FILE *cp) {
	if (cp) {
		fprintf(cp, "%u,", v);
	}
}

void decode_fc1(uint8_t *pkt, FILE *cp) {
	uint32_t v;
	int i;
	printf("MSE temps: ");
	for (i=0; i<4; i++) {
		v = getbits(pkt, (12*i), 12);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	printf("ASIB temps: ");
	for (i=0; i<4; i++) {
		v = getbits(pkt, 48+(10*i), 10);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	printf("PV volts: ");
	for (i=0; i<3; i++) {
		v = getbits(pkt, 88+(16*i), 16);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	v = getbits(pkt, 136, 16);
	printf("PV curr: %u ", v);
	csvprint(v, cp);
	v = getbits(pkt, 136+16, 16);
	printf("Batt V: %u ", v);
	csvprint(v, cp);
	v = getbits(pkt, 136+16+16, 16);
	printf("Sys curr: %u\n", v);
	if (cp)
		fprintf(cp, "%u", v);
}

void decode_ukube(uint8_t *pkt, FILE *cp) {
	uint32_t v;
	int i, b, o;
	printf("MSE temps: ");
	for (i=0; i<4; i++) {
		v = getbits(pkt, (12*i), 12);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	printf("Solar temps: ");
	for (i=0; i<5; i++) {
		v = getbits(pkt, 48+(8*i), 8);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	o = 88;
	for (b=0; b<3; b++) {
		if (b>0) {	// battery 0 has no direction bit
			v = getbits(pkt, o, 1);
			o += 1;
			printf("Bat%d dir: %d ", b, v);
			csvprint(v, cp);
		}
		v = getbits(pkt, o, 8);
		o += 8;
		printf("Bat%d curr: %d ", b, v);
		csvprint(v, cp);
		v = getbits(pkt, o, 8);
		o += 8;
		printf("Bat%d cell: %d ", b, v);
		csvprint(v, cp);
		v = getbits(pkt, o, 8);
		o += 8;
		printf("Bat%d volt: %d ", b, v);
		csvprint(v, cp);
		if (b<2) {	// battery 2 has no temperature data
			v = getbits(pkt, o, 8);
			o += 8;
			printf("Bat%d temp: %d ", b, v);
			csvprint(v, cp);
		}
	}
	v = getbits(pkt, 178, 6);
	printf("Pad: 0x%02x\n", v);
	if (cp)
		fprintf(cp, "%02x", v);
}

void decode_nayif(uint8_t *pkt, FILE *cp) {
	uint32_t v;
	int i;
	printf("CPU temp: ");
	v = getbits(pkt, 0, 12);
	printf("%u ", v);
	csvprint(v, cp);
	printf("RF temp: ");
	v = getbits(pkt, 12, 12);
	printf("%u ", v);
	csvprint(v, cp);
	printf("PA temp: ");
	v = getbits(pkt, 24, 12);
	printf("%u ", v);
	csvprint(v, cp);
	printf("Solar temps: ");
	for (i=0; i<6; i ++) {
		v = getbits(pkt, 36+(i*10), 10);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	printf("Sun sens: ");
	for (i=0; i<6; i ++) {
		v = getbits(pkt, 96+(i*10), 10);
		printf("%d: %u ", i, v);
		csvprint(v, cp);
	}
	printf("Batt temp: ");
	v = getbits(pkt, 156, 8);
	printf("%u ", v);
	csvprint(v, cp);
	printf("Photo cur: ");
	v = getbits(pkt, 164, 10);
	printf("%u ", v);
	csvprint(v, cp);
	printf("Batt volts: ");
	v = getbits(pkt, 174, 14);
	printf("%u ", v);
	csvprint(v, cp);
	printf("Sys curr: ");
	v = getbits(pkt, 188, 12);
	printf("%u\n", v);
	if (cp)
		fprintf(cp, "%u", v);
}

int usage() {
	puts("usage: woddump [-i <wod stream file>] [-f[uncube1]] [-u[kube]] [-n[ayif]] [-c <csv output>]");
	return 0;
}

typedef void (*decode_ptr)(uint8_t*, FILE *);

int main(int argc, char **argv) {
	char *wod = "test.wod";
	char *csv = NULL, *csv_hdrs = "NONE\n";
	decode_ptr decode = NULL;
	FILE *fp, *cp = NULL;
	uint8_t pkt[25];
	int siz = 0, cnt = 0, arg;

	for (arg=1; arg<argc; arg++) {
		if (!strncmp(argv[arg], "-h", 2)) {
			return usage();
		} else if (!strncmp(argv[arg], "-i", 2)) {
			wod = argv[++arg];
		} else if (!strncmp(argv[arg], "-c", 2)) {
			csv = argv[++arg];
		} else if (!strncmp(argv[arg], "-f", 2)) {
			siz = 23;
			cnt = 104;
			csv_hdrs = csv_fc1;
			decode = decode_fc1;
		} else if (!strncmp(argv[arg], "-u", 2)) {
			siz = 23;
			cnt = 104;
			csv_hdrs = csv_ukube;
			decode = decode_ukube;
		} else if (!strncmp(argv[arg], "-n", 2)) {
			siz = 25;
			cnt = 96;
			csv_hdrs = csv_nayif;
			decode = decode_nayif;
		}
	}
	fprintf(stderr, "dumping: %s\n", wod);
	fp = fopen(wod, "rb");
	if (!fp) {
		perror("opening wod file");
		return 1;
	}
	if (csv) {
		cp = fopen(csv, "wb");
		if (!cp) {
			perror("opening CSV file");
			return 1;
		} else {
			fputs(csv_hdrs, cp);
		}
	}
	arg = 0;
	while (arg<cnt && fread(pkt, siz, 1, fp)==1) {
		printf("CNT: %d ", arg);
		if (cp) fprintf(cp, "%d,", arg);
		if (decode) decode(pkt, cp);
		if (cp) fprintf(cp, "\n");
		++arg;
	}
	if (cnt != 96) {
		if (fread(pkt, 8, 1, fp) == 1)
			printf("Callsign: %.8s\n", pkt);
		else
			fprintf(stderr, "cannot read callsign trailer\n");
	}
	fclose(fp);
	if (cp)
		fclose(cp);
	return 0;
}
