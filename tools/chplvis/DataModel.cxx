/*
 * Copyright 2015 Cray Inc.
 * Other additional copyright holders may be indicated within.
 *
 * The entirety of this work is licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "DataModel.h"

// FLTK includes
#include <FL/fl_ask.H>

// C libraries

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void DataModel::newList()
{
  //printf ("newList ...\n");
  curEvent = theEvents.begin();
  while (curEvent != theEvents.end()) {
    //Event *e = *curEvent;
    curEvent = theEvents.erase(curEvent);
    //if (e) delete e;
  }
}

int DataModel::LoadData(const char * filename)
{
  const char *suffix = strrchr(filename, '-');
  if (!suffix) {
    fprintf (stderr,
	     "LoadData: file %s does not appear to be generated by Chapel\n",
	     filename);
    return 0;
  }
  suffix += 1;
  int namesize = strlen(filename) - strlen(suffix);
  // int fileno = atoi(suffix);

  // printf ("LoadData:  namesize is %d, fileno is %d\n", namesize, fileno);

  printf ("loading data from %.*s* files ...", namesize, filename);
  fflush(stdout);

  newList();
  curEvent = theEvents.begin();
  
  FILE *data = fopen(filename, "r");
  if (!data) {
    fprintf (stderr, "LoadData: Could not open %s.\n", filename);
    return 0;
  }

  // Read the config data
  char configline[100];

  if (fgets(configline, 100, data) != configline) {
    fprintf (stderr, "LoadData: Could not read file %s.\n", filename);
    fclose(data);
    return 0;
  }

  // The configuration data
  int nlocales;
  int fnum;
  double seq;

  if (sscanf(configline, "ChplVdebug: nodes %d, id %d, seq %lf",
	     &nlocales, &fnum, &seq) != 3) {
    fprintf (stderr, "LoadData: incorrect data on first line of %s.\n",
	     filename);
    fclose(data);
    return 0;
  }
  fclose(data);

  char fname[namesize+15];
  // printf ("LoadData: nlocalse = %d, fnum = %d seq = %.3lf\n", nlocales, fnum, seq);

  // Set the number of locales.
  numLocales = nlocales;

  for (int i = 0; i < nlocales; i++) {
    snprintf (fname, namesize+15, "%.*s%d", namesize, filename, i);
    if (!LoadFile(fname, i, seq)) {
      fprintf (stderr, "Error processing data from %s\n", fname);
      numLocales = -1;
      return 0;
    }
  }

  printf (" done.\n");
  printf ("list has %ld items\n", (long)theEvents.size());

  return 1;
}


// Load the data in the current file

int DataModel::LoadFile (const char *filename, int index, double seq)
{
  FILE *data = fopen(filename, "r");
  char line[1024];

  int floc;
  int findex;
  double fseq;

  int  nErrs = 0;

  if (!data) return 0;

  // printf ("LoadFile %s\n", filename);
  if (fgets(line,1024,data) != line) {
    fprintf (stderr, "Error reading file %s.\n", filename);
    return 0;
  }

  // First line is the information line ... 

  if (sscanf(line, "ChplVdebug: nodes %d, id %d, seq %lf",
	     &floc, &findex, &fseq) != 3) {
    fprintf (stderr, "LoadData: incorrect data on first line of %s.\n",
	     filename);
    fclose(data);
    return 0;
  }

  // Verify the data

  if (floc != numLocales || findex != index || fabs(seq-fseq) > .01 ) {
    printf ("Mismatch %d = %d, %d = %d, %.3lf = %.3lf\n", floc, numLocales, findex,
	    index, fseq, seq);
    fprintf (stderr, "Data file %s does not match selected file.\n", filename);
    return 0;
  }

  // Now read the rest of the file
  std::list<Event *>::iterator itr = theEvents.begin();

  while ( fgets(line, 1024, data) == line ) {
    // Common Data
    char *linedata;
    long sec;
    long usec;
    long nextCh;

    // Data for tasks and comm and fork
    int nid;    // Node id
    int ntll;   // Node task list locale
    char nbstr[10];  // "begin" or "nb"
    int nlineno; // line number starting the task
    char nfilename[512];  // File name starting the task

    // comm
    int isGet;  // put (0), get (1)  currently ignoring non-block and strid
    int rnid;   // remote id
    long locAddr;  // local address
    long remAddr;  // remote address
    int eSize;     // element size
    int typeIx;    // type Index
    int dlen;      // data length 

    // fork
    int fid;
    
    // Process the line
    linedata = strchr(line, ':');
    if (linedata ) {
      if (sscanf (linedata, ": %ld.%ld%ln", &sec, &usec, &nextCh) != 2) {
	printf ("Can't read time from '%s'\n", linedata);
      }
    } else {
      nErrs++;
      continue;
    }

    Event *newEvent = NULL;
    
    //printf("%c", line[0]);
    switch (line[0]) {

      case 0:  // Bug in output???
        nErrs++;
	break;

      case 't':  // new task line
	//  task: s.u nodeID task_list_locale begin/nb lineno filename
	if (sscanf(&linedata[nextCh], "%d %d %9s %d %511s",
		   &nid, &ntll, nbstr, &nlineno, nfilename) != 5) {
	  fprintf (stderr, "Bad task line: %s\n", filename);
	  fprintf (stderr, "nid = %d, ntll = %d, nbstr = '%s', nlineno = %d"
		   " nfilename = '%s'\n", nid, ntll, nbstr, nlineno, nfilename);
	  nErrs++;
	} else {
	  newEvent = new E_task(sec, usec, ntll);
	}
	break;

      case 'e':  // end task (not generated yet)
        //printf ("z");
	break;

      case 'n':  // non-blocking put or get
      case 's':  // strid put or get
      case 'g':  // regular get
      case 'p':  // regular put
	// All comm data: 
	// s.u nodeID otherNode loc-addr rem-addr elemSize typeIndex len lineno filename
	if (sscanf(&linedata[nextCh], "%d %d 0x%lx 0x%lx %d %d %d %d %511s",
		   &nid, &rnid, &locAddr, &remAddr, &eSize, & typeIx, &dlen,
		   &nlineno, nfilename) != 9) {
	  fprintf (stderr, "Bad comm line: %s\n", filename);
	  nErrs++;
	} else {
	  isGet = (line[0] == 'g' ? 1 :
		   line[0] == 'p' ? 0 :
		   line[3] == 'g' ? 1 : 0);
	  if (isGet)
	    newEvent = new E_comm(sec, usec, rnid, nid);
	  else
	    newEvent = new E_comm(sec, usec, nid, rnid);
	}
	break;

      case 'f':  // All the forks:
	// s.u nodeID otherNode subloc fid arg arg_size
	if (sscanf(&linedata[nextCh], "%d %d %d", 
		    &nid, &rnid, &fid) != 3) {
	  fprintf (stderr, "Bad fork line: %s\n", filename);
	  nErrs++;
	} else {
	  newEvent = new E_fork(sec, usec, nid, rnid, line[1] == '_');
	}
	break;

      case 'm':  // Tag in the data
	printf ("-%ld-", sec);
	nextCh++;
	newEvent = new E_tag(sec,&linedata[nextCh]);
	break;
      
      default:
	/* Do nothing */ ;
	//printf ("d");
    }
    //  Add the newEvent to the list
    if (newEvent) {
      if (theEvents.empty()) {
	printf ("B");
	theEvents.push_front(newEvent);
      } else {
	if (itr == theEvents.end()) {
	  printf("E");
	  theEvents.insert(itr, newEvent);
	} else {
	  while (itr != theEvents.end()
		 && (*itr)->Ekind() != Ev_tag 
	         && *itr < newEvent)
	    itr++;
	  if (itr != theEvents.end() && (*itr)->Ekind() == Ev_tag) {
	    if (newEvent->Ekind() != Ev_tag) {
	      printf("+");
	      theEvents.insert(itr, newEvent);
	    } else {
	      if ((*itr)->tsec() == newEvent->tsec()) {
		printf("x");
		itr++;  // Move past the tag.
	      } else {
		fprintf (stderr, "Data Error: tag missmatch. %ld vs %ld\n",
			 (*itr)->tsec(), newEvent->tsec());
	      }
	    }
	  } else {
	    printf(".");
	    theEvents.insert(itr, newEvent);
	  }
	}
      }
    }
  }
  printf ("\n");

  if (nErrs) fprintf(stderr, "%d errors in data file '%s'.\n", nErrs, filename);
  
  if ( !feof(data) ) return 0;
  
  return 1;
}
