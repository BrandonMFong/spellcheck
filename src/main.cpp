/**
 * author: brando
 * date: 1/9/24
 */

#include <stdio.h>
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

extern "C" {
#include <bflibc/bflibc.h>
}

/**
 * default dictionary location will be local
 */
#define WORDS_DICTIONARY_REF_DEFAULT "/usr/share/dict/words"
#define BRIEF_DESCRIPTION_ARG "--brief-description"

using namespace BF;

typedef struct {
	/**
	 * where the dictionary of words are
	 */
	String wordsref;

	/**
	 * either a file path or string
	 *
	 * this will be automatically detected in function `...GetSubject`
	 */
	String subject;
} SpellcheckTools;

void SpellcheckToolsSetDefault(SpellcheckTools * opts) {
	if (opts) return;
	opts->subject = "";
	opts->wordsref = WORDS_DICTIONARY_REF_DEFAULT;
}

void Help(const char * toolname) {
	printf("usage: %s <subject>\n", toolname);

	printf("\nparameters:\n");
	printf("\n  <subject> : either text or file path\n");

	printf("\nCopyright © 2024 Brando. All rights reserved.\n");
}

void BriefDescription() {
	printf("checks spelling of word or content\n");
}

int ParseArguments(int argc, char ** argv, SpellcheckTools * opts, bool * brieflyDescribe);
int Spellcheck(const SpellcheckTools * tools);

int main(int argc, char ** argv) {
	int error = 0;
	SpellcheckTools tools;

	SpellcheckToolsSetDefault(&tools);

	bool brieflyDescribe = false;
	error = ParseArguments(argc, argv, &tools, &brieflyDescribe);

	if (error) {
		Help(argv[0]);
	} else if (brieflyDescribe) {
		BriefDescription();
	} else {
		error = Spellcheck(&tools);
	}

	return error;
}

int ParseArguments(int argc, char ** argv, SpellcheckTools * tools, bool * brieflyDescribe) {
	int error = 0;
	if (!argv || !tools || !brieflyDescribe || (argc < 2)) return -2;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], BRIEF_DESCRIPTION_ARG)) {
			*brieflyDescribe = true;
		} else if (i == (argc-1)) { // subject
			tools->subject = argv[i];
		}
	}

	return 0;
}

int WordsLoad(const SpellcheckTools * tools, Tree<String> & words) {
	FILE * file = fopen(WORDS_DICTIONARY_REF_DEFAULT, "r");
	if (file == NULL) return -3;

	char line[1024];
	size_t s = 0;
	while (fgets(line, sizeof(line), file) != NULL) {
		line[strcspn(line, "\n")] = '\0';
		String word(line);
		words.insert(word);
		s++;
	}

	return 0;
}

/**
 * determines if subject is a file or not
 */
int SpellcheckToolsGetSubject(const SpellcheckTools * tools, String & text) {
	if (!tools) return 1;
	if (BFFileSystemPathIsFile(tools->subject)) {
		// todo: get file content
		return text.readFromFile(tools->subject);
	} else {
		text = tools->subject;
	}
	return 0;
}

/**
 * Loads text referenced by the spellcheck tools
 */
int TextLoad(const SpellcheckTools * tools, String & text) {
	return SpellcheckToolsGetSubject(tools, text);
}

int WordCompare(const String & a, const String & b) {
	return a.compareString(b);
}

// Function to calculate minimum of three values
int min(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

// Function to calculate Levenshtein distance between two words
int LevenshteinDistance(const char *word1, const char *word2) {
    int m = strlen(word1);
    int n = strlen(word2);

    // Create a matrix to store the distances
    int dp[m + 1][n + 1];

    // Initialize the matrix
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (word1[i - 1] == word2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
        }
    }

    // Return the final value (bottom-right cell of the matrix)
    return dp[m][n];
}

/**
 * if true, word is spelled correctly
 */
bool SpellcheckWord(const String & word, const SpellcheckTools * tools, const Tree<String> & words) {
	if (!tools) return -6;

	Tree<String>::Node n = words.root();
	bool found = false;
	while (!n.isNull() && !found) {
		int cmp = WordCompare(word, n.object());
		if (cmp == 0) { // text == n
			found = true;
		} else if (cmp < 0) { // text < n
			n = n.left();
		} else if (cmp > 0) { // text > n
			n = n.right();
		}
	}

	return found;
}

typedef struct {
	char * buf;
} SpellcheckTextTools;

int SpellcheckTextToolsStart(SpellcheckTextTools * tools, const String & text) {
	if (!tools) return -20;

	tools->buf = text.cStringCopy();

	return 0;
}

int SpellcheckTextToolsGetWord() {
	return 0;
}

int SpellcheckTextToolsGetStop() {
	return 0;
}

/**
 * echos text with notations that show misspelled words
 */
int SpellcheckText(
	const String & text,
	const SpellcheckTools * tools,
	const Tree<String> & words
) {
	SpellcheckTextTools t;

	//int error = SpellcheckTextToolsStart(&tools, text);
	char * txtcpy = text.cStringCopy();
	char * word = strtok(txtcpy, " ");
	while (word) {
		if (SpellcheckWord(word, tools, words)) { // correct
			printf("%s ", word);
		} else { // incorrect
			printf("{%s} ", word);
		}
		word = strtok(0, " ");
	}

	BFFree(txtcpy);

	printf("\n");

	return 0;
}

int Spellcheck(const SpellcheckTools * tools) {
	String text;
	int error = TextLoad(tools, text);

	Tree<String> words;
	words.setCompare(WordCompare);
	if (!error) {
		error = WordsLoad(tools, words);
	}
	
	if (!error) {
		SpellcheckText(text, tools, words);
	}

	return -5;
}

/*
#ifdef TESTING

#include <bflibcpp/bftests.hpp>

int test_LevenshteinDistance(void) {
	UNIT_TEST_START;
	int result = 0;

	String a = "kitten";
	String b = "spitting";
	int d = LevenshteinDistance("kitten", "spitting");
	if (d != 4) result = 1;

	d = LevenshteinDistance("hello", "hellp");
	if (d != 1) result = 2;

	UNIT_TEST_END(!result, result);
	return result;
}

int TOOL_TEST(int argc, char ** argv) {
	int p = 0, f = 0;
	printf("TESTING: %s\n", argv[0]);

	LAUNCH_TEST(test_LevenshteinDistance, p, f);

	PRINT_GRADE(p, f);

	return 0;
}

#endif // TESTING
*/
