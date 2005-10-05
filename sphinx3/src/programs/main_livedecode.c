/* ====================================================================
 * Copyright (c) 1999-2004 Carnegie Mellon University.	All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
 /*************************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 2000 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 *************************************************
 *
 *  Aug 6, 2004
 *
 * HISTORY
 *
 * $Log$
 * Revision 1.11  2005/10/05  00:31:14  dhdfu
 * Make int8 be explicitly signed (signedness of 'char' is
 * architecture-dependent).  Then make a bunch of things use uint8 where
 * signedness is unimportant, because on the architecture where 'char' is
 * unsigned, it is that way for a reason (signed chars are slower).
 * 
 * Revision 1.10  2005/06/22 05:39:56  arthchan2003
 * Synchronize argument with decode. Removed silwid, startwid and finishwid.  Wrapped up logs3_init, Wrapped up lmset. Refactor with functions in dag.
 *
 * Revision 1.2  2005/03/30 00:43:41  archan
 * Add $Log$
 * Revision 1.11  2005/10/05  00:31:14  dhdfu
 * Make int8 be explicitly signed (signedness of 'char' is
 * architecture-dependent).  Then make a bunch of things use uint8 where
 * signedness is unimportant, because on the architecture where 'char' is
 * unsigned, it is that way for a reason (signed chars are slower).
 * 
 * Add Revision 1.10  2005/06/22 05:39:56  arthchan2003
 * Add Synchronize argument with decode. Removed silwid, startwid and finishwid.  Wrapped up logs3_init, Wrapped up lmset. Refactor with functions in dag.
 * Add into most of the .[ch] files. It is easy to keep track changes.
 *
 */

/** \file main_livedecode.c
 * \brief live-mode decoder demo. 
 *
 *  Created by Yitao Sun (yitao@cs.cmu.edu).  This is a test program written
 *  for the Win32 platform.  The program initializes Sphinx3 live-decode API,
 *  then in a press-to-start and press-to-stop fashion, records and decodes a 
 *  session of user speech.  The threading and synchronization code are Win32-
 *  specific.  Ravi Mosur (rkm@cs.cmu.edu) suggested using select() (and no
 *  threads) on the /dev/tty* device to remove Win32 dependency.
 */

#include <live_decode_API.h>
#include <live_decode_args.h>
#include <ad.h>
#include <stdio.h>

#define BUFSIZE 4096

int
main(int argc, char **argv)
{
  char *hypstr;
  int rv;
  ad_rec_t *in_ad = 0;
  int16 samples[BUFSIZE];
  int32 num_samples;
  live_decoder_t decoder;
  FILE *dump = 0;

  /*
   * Initializing
   */
  if (argc != 2) {
    printf("Usage:  livedecode config_file \n");
    return -1;
  }

  if (cmd_ln_parse_file(arg_def, argv[1])) {
    printf("Bad arguments file (%s).\n", argv[1]);
    return -1;
  }

  if (ld_init(&decoder)) {
    printf("Initialization failed.\n");
    return -1;
  }

  if (ld_begin_utt(&decoder, 0)) {
    printf("Cannot start decoding\n");
    return -1;
  }

  /** initializing a file to dump the recorded audio */
  if ((dump = fopen("out.raw", "wb")) == 0) {
    printf("Cannot open dump file out.raw\n");
    return -1;
  }

  in_ad = ad_open_sps(cmd_ln_int32 ("-samprate"));
  ad_start_rec(in_ad);
  while (1) {
      num_samples = ad_read(in_ad, samples, BUFSIZE);
      printf("read %d samples\n", num_samples);
      if (num_samples > 0) {
	/** dump the recorded audio to disk */
	if (fwrite(samples, sizeof(int16), num_samples, dump) < num_samples) {
	  printf("Error writing audio to dump file.\n");
	}
	ld_process_raw(&decoder, samples, num_samples);
      }
  }
  ad_stop_rec(in_ad);
  ad_close(in_ad);
  ld_end_utt(&decoder);

  /*
   *  Print the decoding output
   */
  if (ld_retrieve_hyps(&decoder, NULL, &hypstr, NULL)) {
    printf("Cannot retrieve hypothesis.\n");
  }
  else {
    printf("Hypothesis:\n%s\n", hypstr);
  }

  ld_finish(&decoder);
  
  fclose(dump);

  return 0;
}
