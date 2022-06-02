#pragma warning(disable : 4996)

#include <stdio.h>
#include <string.h>

#define DEBUG_WILDCARD
#ifdef DEBUG_WILDCARD
void printIndent(int debugDepth)
{
    printf("  [%d] ", debugDepth);
    for (int i = 0; i < debugDepth; ++i) {
        printf("  ");
    }
    printf("* ");
}
#endif

bool wildcardMatches(const char * wildcard, const char * str, int debugDepth = 0)
{
#ifdef DEBUG_WILDCARD
    printIndent(debugDepth);
    printf("wildcardMatches(\"%s\", \"%s\", %d)\n", wildcard, str, debugDepth);
#endif

    const char * wildcardPos = strchr(wildcard, '*');
    if (!wildcardPos) {
        return !strcmp(wildcard, str);
    }

    const size_t prefixLen = wildcardPos - wildcard;
    if (strncmp(wildcard, str, prefixLen)) {
        return false;
    }

    const size_t strLen = strlen(str);
    const char * remainingWildcard = wildcardPos + 1;
    const size_t remainingWildcardLen = strlen(remainingWildcard);

    size_t skipCount = remainingWildcardLen;
    for (const char * p = remainingWildcard; *p; ++p) {
        if (*p == '*') {
            // Don't count '*', they are zero-width
            --skipCount;
        }
    }

    if (skipCount > strLen) {
        return false;
    }

    const char * p = str + strLen - skipCount;
    for (; p >= str; --p) {
        if (wildcardMatches(remainingWildcard, p, debugDepth + 1)) {
            return true;
        }
    }
    return false;
}

void test(const char * wildcard, const char * str, bool expectation)
{
    bool matched = wildcardMatches(wildcard, str);
    printf("-> [%s] W:\"%s\" S:\"%s\": %s\n", (matched == expectation) ? "pass" : "FAIL", wildcard, str, matched ? "Match" : "No Match");
}

int main(int argc, char * argv[])
{
    test("example.com", "example.com", true);
    test("example.com", "nomatch.example.com", false);
    test("yeet.*.example.com", "nomatch.example.com", false);
    test("foo.*.example.com", "foo.bar.example.com", true);
    test("*.*.example.com", "foo.bar.example.com", true);
    test("*", "matches.anything.at.all.example.com", true);
    test("*", "", true);
    test("", "", true);
    test("****", "also.matches.anything.at.all.example.com", true);
    test("****", "a", true);
    test("a****", "a", true);
    test("****a", "a", true);
    test("**a*", "a", true);
    test("****b", "a", false);
    test("**b**", "a", false);
    return 0;
}
