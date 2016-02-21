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
 
static char *csv_wod="CNT,MSE0,MSE1,MSE2,MSE3,AMAC0,AMAC1,AMAC2,AMAC3,AMAC4,"
	"BCUR0,BCEL0,BVLT0,BTMP0,BDIR1,BCUR1,BCEL1,BVLT1,BTMP1,BDIR2,BCUR2,BCEL2,BVLT2,PAD\n";

void csvprint(uint32_t v, FILE *cp) {
	if (cp) {
		fprintf(cp, "%u,", v);
	}
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

int main(int argc, char **argv) {
	char *wod = "test.wod";
	char *csv = NULL, *csv_hdrs = "NONE\n";
	FILE *fp, *cp = NULL;
	uint8_t pkt[23];
	int arg;

	for (arg=1; arg<argc; arg++) {
		if (!strncmp(argv[arg], "-i", 2)) {
			wod = argv[++arg];
		} else if (!strncmp(argv[arg], "-c", 2)) {
			csv = argv[++arg];
			csv_hdrs = csv_wod;
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
	while (arg<104 && fread(pkt, sizeof(pkt), 1, fp)==1) {
		printf("CNT: %d ", arg);
		if (cp) fprintf(cp, "%d,", arg);
		++arg;
		decode_ukube(pkt, cp);
		if (cp) fprintf(cp, "\n");
	}
	if (fread(pkt, 8, 1, fp) == 1)
		printf("Callsign: %.8s\n", pkt);
	else
		fprintf(stderr, "cannot read callsign trailer\n");
	fclose(fp);
	if (cp)
		fclose(cp);
	return 0;
}
