/*
* lmsridll.h  -  Language Model dll, we rewrite the interface of sri toolkit
*
* Copyright (C) 2006 by Hongxu Hou <hxhou@ict.ac.cn> 
Multilingual Interaction Technology and Evaluation Laboratory, ICT, CAS

* Begin       : 04/13/2006
* Last Change : 04/13/2006
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef __LMSRCDLL_H
#define __LMSRCDLL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#ifdef LMSRIDLL_EXPORTS
#define LMSRIDLL_API __declspec(dllexport)
#else
#define LMSRIDLL_API __declspec(dllimport)
#endif
#else
#define LMSRIDLL_API
#endif

extern "C" {

// Load Language Model
// fn: language model file name, support two types: text file or zip file (extend file name is gz)
// arpa: ARPA format or Binary format:  0 for Binary format, !0 for ARPA format
// order: n-gram, default is 3-gram
// unk: if hold the unknown words <unk> or not, default is not
// tolow: if change the upper letter to lower letter or not, default is not
// return the LM pointer
LMSRIDLL_API void * sriLoadLM(const char * fn, int arpa=0, int order=3, int unk=0, int tolow=0);

// Set the maximum order
// plm: LM pointer
// o: new order
// return the original order
LMSRIDLL_API int sriSetOrder(void * plm, int o);

// unload the LM
// plm: LM pointer
LMSRIDLL_API void sriUnloadLM(void * plm);

// compute the sentence LM perplexity
// plm: LM pointer
// sentence: 
// return perplexity
//     perplexity(a1 a2 a3 ... an)=log10(P(a1)*P(a2|a1)*P(a3|a1a2)*...*P(an|a1...an-1))
LMSRIDLL_API double sriPerplexity(void * plm, const char * sentence);

// compute n-gram
// plm: LM pointer
// return: log10(P(word|context))
LMSRIDLL_API double sriWordProb(void * plm, const char * word, const char * context);

} // extern "C"
#endif
