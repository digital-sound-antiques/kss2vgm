#include "kss2vgm.h"
#include "kssplay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HLPMSG                                                                                                         \
  "Usage: kss2vgm [Options] kssfile \n"                                                                                \
  "Options: \n"                                                                                                        \
  "  -p<play_time>  Specify play length in second to convert.\n"                                                       \
  "  -s<song_num>   Internal song number to play.\n"                                                                   \
  "  -o<file>       Specify the output filename.\n"                                                                    \
  "  -v<num>        Volume modifier: Volume = 2^(num/32).\n"                                                           \
  "  -l<num>        Maximum loop number, works only with .mgs file (default: 0 = infinite).\n"

#define MAX_PATH 512

typedef struct {
  int song_num;
  int play_time;
  int loop_num;
  int volume;
  char input[MAX_PATH + 4];
  char output[MAX_PATH + 4];
  int help;
  int error;
} Options;

static Options parse_options(int argc, char **argv) {

  Options options;
  int i, j;

  options.song_num = 0;
  options.play_time = 60;
  options.loop_num = 0;
  options.input[0] = '\0';
  options.output[0] = '\0';
  options.help = 0;
  options.error = 0;
  options.volume = 0x0;

  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'p':
        options.play_time = atoi(argv[i] + 2);
        break;
      case 's':
        options.song_num = atoi(argv[i] + 2);
        break;
      case 'l':
        options.loop_num = atoi(argv[i] + 2);
        if (options.loop_num == 0) {
          options.loop_num = 256;
        }
        break;
      case 'v':
        options.volume = atoi(argv[i] + 2);
        break;
      case 'o':
        for (j = 0; argv[i][j + 2]; j++) {
          options.output[j] = argv[i][j + 2];
        }
        options.output[j] = '\0';
        break;
      default:
        options.error = 1;
        break;
      }
    } else {
      strncpy(options.input, argv[i], MAX_PATH);
    }
  }

  if (options.output[0] == '\0') {
    char *dot = strrchr(options.input, '.');
    if (dot == NULL) {
      strcpy(options.output, options.input);
    } else {
      char *rp = options.input;
      i = 0;
      while (rp < dot) {
        options.output[i++] = *(rp++);
      }
      options.output[i] = '\0';
    }
    strcat(options.output, ".vgm");
  }

  return options;
}

static void fprint_chip_info(FILE *fp, KSS2VGM *kss2vgm) {
  fprintf(fp, "Used chips: ");

  if (kss2vgm->use_psg) {
    fprintf(fp, "AY-3-8910 ");
  }
  if (kss2vgm->use_scc_plus) {
    fprintf(fp, "K052539(SCC+) ");
  } else if (kss2vgm->use_scc) {
    fprintf(fp, "K051649(SCC) ");
  }
  if (kss2vgm->use_opll) {
    fprintf(fp, "YM2413 ");
  }
  if (kss2vgm->use_opl) {
    if (kss2vgm->use_y8950_adpcm) {
      fprintf(fp, "Y8950+ADPCM ");
    } else {
      fprintf(fp, "Y8950 ");
    }
  }
  if (kss2vgm->use_sng) {
    fprintf(fp, "SN76489 ");
  }

  fprintf(fp, "\n");
}

int main(int argc, char **argv) {

  FILE *fp;

  if (argc < 2) {
    fprintf(stderr, HLPMSG);
    exit(0);
  }

  Options opt = parse_options(argc, argv);

  if (opt.error) {
    fprintf(stderr, HLPMSG);
    exit(1);
  }

  KSS *kss = KSS_load_file(opt.input);
  if ((kss = KSS_load_file(opt.input)) == NULL) {
    fprintf(stderr, "File read error");
    exit(1);
  }

  KSS2VGM *kss2vgm = KSS2VGM_new();
  KSS2VGM_Result *res = KSS2VGM_kss2vgm(kss2vgm, kss, opt.play_time * 1000, opt.song_num, opt.loop_num, opt.volume);

  /* Open output VGM file */
  if ((fp = fopen(opt.output, "wb")) == NULL) {
    fprintf(stderr, "Can't open %s\n", opt.output);
    exit(1);
  }

  fwrite(res->vgm, sizeof(uint8_t), res->vgm_size, fp);

  fprintf(stdout, "Output: %s\n", opt.output);
  fprint_chip_info(stdout, kss2vgm);
  fprintf(stdout, "Size: %d bytes\n", res->vgm_size);

  KSS2VGM_delete(kss2vgm);
  KSS2VGM_Result_delete(res);
  KSS_delete(kss);

  return 0;
}

