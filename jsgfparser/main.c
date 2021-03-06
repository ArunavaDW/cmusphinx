/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2007 Carnegie Mellon University.  All rights
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

#include <hash_table.h>
#include <err.h>

#include <string.h>

#include "jsgf.h"

static int
write_fsg(jsgf_t *grammar, const char *name)
{
    glist_t rules;
    int32 nrules;
    gnode_t *gn;

    rules = hash_table_tolist(grammar->rules, &nrules);
    for (gn = rules; gn; gn = gnode_next(gn)) {
        hash_entry_t *he = gnode_ptr(gn);
        jsgf_rule_t *rule = hash_entry_val(he);

        if ((name == NULL && rule->public)
            || (name && 0 == strncmp(rule->name + 1, name, strlen(rule->name) - 2))) {
            jsgf_write_fsg(grammar, rule, stdout);
            break;
        }
    }
    glist_free(rules);

    return 0;
}

int
main(int argc, char *argv[])
{
    int yyrv;
    jsgf_t *jsgf;

    jsgf = jsgf_parse_file(argc > 1 ? argv[1] : NULL, NULL);
    if (jsgf == NULL) {
        return 1;
    }
    write_fsg(jsgf, argc > 2 ? argv[2] : NULL);

    return yyrv;
}
