#ifndef __PERPLEXITY_H
#define __PERPLEXITY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void * sriLoadLM(const char * fn, int arpa=0, int order=3, int unk=0, int tolow=0);
void sriUnloadLM(void * plm);
double sriPerplexity(void * plm, const char * sentence);
double sriWordProb(void * plm, const char * word, const char * context);

#endif

