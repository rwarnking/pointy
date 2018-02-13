#ifndef _TESTER_H_
#define _TESTER_H_

void TestCliques(const char *file);
void TestDir(const char *indir, const char *outfile, int iterations=1, bool draw=false, bool cliques=false);
bool TestFile(const char *infile, const char *outfile, int iterations=1, bool draw=false, bool cliques=false);

#endif /* _TESTER_H_ */