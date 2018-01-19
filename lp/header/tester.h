#ifndef _TESTER_H_
#define _TESTER_H_

void TestDir(const char *indir, const char *outfile, int iterations=1);
bool TestFile(const char *infile, const char *outfile, int iterations=1);

#endif /* _TESTER_H_ */