/*
 ============================================================================
 Name        : GWeinrodHtmlSrcList.c
 Author      : Gregory Weinrod
 Version     :
 Copyright   : 2017
 Description : This short program analyzes the xml src attributes and tags of
 an html web page provided by the user.  Commands are c - count valid src
 attributes, t - print all tags associated with valid src attributes,
 u - print all URLs associated with valid src attributes, f - list each unique
 tag with the frequency of each.
 ============================================================================
 */

//TODO function pointer for type of assessment
//TODO replace memsets with assignment of dereferenced pointer value = '\0'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void count(const char *pwebSource);
void tags(const char *pwebSource);
void urls(const char *pwebSource);
void freq(const char *pwebSource);

int main(void) {
	setvbuf(stdout, NULL, _IONBF, 0);		//turn off output buffering

	char buf[BUFSIZ];
	char webSource[BUFSIZ] = "";
	char curl[BUFSIZ] = "curl -s ";
	char command = ' ';

	printf("%s", "URL:\n");
	fgets(buf, BUFSIZ, stdin);
	buf[strlen(buf) - 1] = '\0';
	strcat(curl, buf);

	FILE *fpwebSource = popen(curl, "r");
	while (fgets(buf, BUFSIZ, fpwebSource) != NULL) {
		strcat(webSource, buf);
		//fgets(buf, BUFSIZ, fpwebSource);
	}
	pclose(fpwebSource);

	/*
	 * Print the html source to a file for debugging purposes
	 *
	 FILE *pSourceOut = fopen("webSource.txt", "w");
	 fprintf(pSourceOut, "%s", webSource);
	 fclose(pSourceOut);
	 */

	while (command != 'q' && command != 'Q') {
		printf("%s\n", "Ready");
		fgets(buf, BUFSIZ, stdin);
		command = buf[0];
		switch (command) {
		case 'c':
		case 'C':
			count(webSource);
			break;
		case 't':
		case 'T':
			tags(webSource);
			break;
		case 'u':
		case 'U':
			urls(webSource);
			break;
		case 'f':
		case 'F':
			freq(webSource);
			break;
		default:
			command = 'q';
			break;
		}

	}
	printf("%s", "Complete\n");
	return EXIT_SUCCESS;
}

/*
 * print the number of valid src attributes found in the webdoc to std out
 */
void count(const char *pwebSource) {
	int count = 0;
	while ((pwebSource = strstr(pwebSource, "src=\"")) != NULL) {
		if ((*(pwebSource - 1) == ' ') || (*(pwebSource - 1) == '\t')
				|| (*(pwebSource - 1) == '\n') || (*(pwebSource - 1) == '\r')) {
			count++;
		}
		pwebSource++;
	}
	printf("%d\n", count);
}

/*
 * each time a valid src attribute is found, print the associated tag name and a newline to stdout
 */
void tags(const char *pwebSource) {
	const char *tagOpen;
	const char *tagEnd;
	char tag[BUFSIZ];
	while ((pwebSource = strstr(pwebSource, "src=\"")) != NULL) {
		if ((*(pwebSource - 1) == ' ') || (*(pwebSource - 1) == '\t')
				|| (*(pwebSource - 1) == '\n') || (*(pwebSource - 1) == '\r')) {
			for (tagOpen = pwebSource; *tagOpen != '<'; tagOpen--)
				;
			for (tagEnd = tagOpen;
					(*tagEnd != ' ') && (*tagEnd != '\t') && (*tagEnd != '\n')
							&& (*tagEnd != '\r'); tagEnd++)
				;
			memset(tag, '\0', sizeof(tag));
			//*tag = '\0';
			strncpy(tag, tagOpen + 1, tagEnd - tagOpen - 1);
			printf("%s\n", tag);
		}
		pwebSource++;
	}

}

/*
 * each time a valid src att is found, print the associated url plus a \n to std out
 */
void urls(const char *pwebSource) {
	const char *qOpen;
	const char *qClose;
	while ((pwebSource = strstr(pwebSource, "src=\"")) != NULL) {
		if ((*(pwebSource - 1) == ' ') || (*(pwebSource - 1) == '\t')
				|| (*(pwebSource - 1) == '\n') || (*(pwebSource - 1) == '\r')) {
			for (qOpen = qClose = pwebSource + 5; *qClose != '"'; qClose++)
				;
			char url[BUFSIZ] = "";
			//memset(url, '\0', sizeof(url));
			strncpy(url, qOpen, qClose - qOpen);
			printf("%s\n", url);
		}
		pwebSource++;
	}

}

void freq(const char *pwebSource) {

	const char *tagOpen;
	const char *tagEnd;
	char tag[BUFSIZ];

	int tagCounts[100] = { 0 };
	char *tagList[100] = { NULL };

	while ((pwebSource = strstr(pwebSource, "src=\"")) != NULL) {

		if ((*(pwebSource - 1) == ' ') || (*(pwebSource - 1) == '\t')
				|| (*(pwebSource - 1) == '\n') || (*(pwebSource - 1) == '\r')) {
			// find a tag, store in 'tag'
			for (tagOpen = pwebSource; *tagOpen != '<'; tagOpen--)
				;
			for (tagEnd = tagOpen;
					(*tagEnd != ' ') && (*tagEnd != '\t') && (*tagEnd != '\n')
							&& (*tagEnd != '\r'); tagEnd++)
				;
			memset(tag, '\0', sizeof(tag));
			strncpy(tag, tagOpen + 1, tagEnd - tagOpen - 1);

			//	look for the tag in our list
			//if found, increment the parallel count, break
			//  if current tag after this loop is null, the tag was not found
			//add it to the list, increment parallel count
			char **currentTag;
			int *currentCount;
			for (currentTag = tagList, currentCount = tagCounts; *currentTag;
					currentTag++, currentCount++) {
				if ((currentTag) && (strcmp(*currentTag, tag) == 0)) {
					(*currentCount)++; //*currentCount += 1; //the ++ has higher precedence than *
					break;
				}

			}
			if (!*currentTag) {
				*currentTag = malloc(strlen(tag) * sizeof(char));
				strncpy(*currentTag, tag, strlen(tag));
				(*currentCount)++;
			}
		}
		pwebSource++;
	}

	/*
	 * walk the arrays and print out tags with counts
	 */
	char **currentTag;
	int *currentCount;
	for (currentTag = tagList, currentCount = tagCounts; *currentTag;
			currentTag++, currentCount++) {
		printf("%s\t%d\n", *currentTag, *currentCount);
		free(*currentTag);	//MUST free all malloc'd memory
	}

}
