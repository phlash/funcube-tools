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
 
static char *csv_hdrs="ID,FRAME,SUN,SCUR0,SCUR1,SCUR2,SCUR3,SCUR4,SCUR5,SCUR6,SCUR7,SCUR8,SCUR9,SCUR10,SCUR11,STEMP,"
	"B0D,B0C,B0CV,B0V,B0T,B1D,B1C,B1CV,B1V,B1T,B2D,B2C,B2CV,B2V,B2T,BHEAT,ATIME,A0S,A1S,A2S,A3S,ATEMP,"
	"RF0,RF1,RF2,RF3,RF4,RF5,PA0,PA1,PA2,PA3,MAG0,MAG1,MAG2,MTEMP,SEQ,CCNT,LCMD,CMDOK\n";

void csvprint(uint32_t v, FILE *cp) {
	if (cp) {
		fprintf(cp, "%u,", v);
	}
}

void decode(uint8_t *pkt, FILE *cp) {
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

void dump(uint8_t *pkt) {
	int i;
	for (i=0; i<56; i++)
		printf("%02x ", pkt[i]);
	printf("\n");
}

int main(int argc, char **argv) {
	char *bin = "tlmtestcapture.funcubebin";
	char *csv = NULL;
	FILE *fp, *cp = NULL;
	uint8_t pkt[256];

	if (argc>1)
		bin = argv[1];
	if (argc>2)
		csv = argv[2];
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
		decode(pkt, cp);
	}
	fclose(fp);
	if (cp)
		fclose(cp);
	return 0;
}
