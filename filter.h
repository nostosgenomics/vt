/* The MIT License

   Copyright (c) 2013 Adrian Tan <atks@umich.edu>

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef FILTER_H
#define FILTER_H

#include "htslib/vcf.h"
#include "variant_manip.h"

#define LT 0
#define LE 1
#define EQ 2
#define GT 3
#define GE 4

/**
 * Class for filtering VCF records.
 * Filters based on several fields:
 * QUAL
 * FILTER
 * INFO
 * VARIANT (inferred)
 *
 * examples
 *
 * QUAL>40
 * FILTER==PASS
 * VARIANT==SNP
 * AF>0.5
 * VARIANT==SNP && AF>0.5

 [-F] filter expression

based on QUAL, FILTER, INFO and Variant type

-f QUAL>2&&PASS&&AF>0.05
-f PASS && AF*>0.05
-f PASS && (AF*>0.05 || AC/AN>0.05)

reserved key words
PASS
QUAL
AF
AC
AN

-f, -g, -h
In the case that a field is not found, it evaluates to false
AF* - intelligent parsing - if AF is not present, estimate from AC/AN
 *
 */
class Node
{
    public:

    Node* parent;
    Node* left;
    Node* right;

    int32_t type;
    bool value;
    
    char* tag;
    union
    {
        bool b;
        int32_t i;
        float f;
    };
    
    /**
     * Evaluates the actions for this node.
     */
    void evaluate(bcf_hdr_t *h, bcf1_t *v, Variant *variant);
};

class Filter
{
    public:

    //encodes the filter expression
    Node* tree;

    //useful pointers
    bcf_hdr_t *h;
    bcf1_t *v;
    Variant *variant;

    Filter() {};

    /**
     * Applies filter to vcf record.
     */
    bool apply(bcf_hdr_t *h, bcf1_t *v, Variant *variant);

    /**
     * Recursive call for apply.
     */
    void apply(Node* node);

    /**
     * Constructs the expression tree.
     */
    void parse(const char* filter);
    
    /**
     * Recursive call for parse.
     */
    void parse(const char* filter, int32_t len);
    
};

#endif