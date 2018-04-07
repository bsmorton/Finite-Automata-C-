// Submitter: bsmorton(Morton, Bradley)
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"

typedef ics::ArrayQueue<std::string>                InputsQueue;
typedef ics::ArrayMap<std::string,std::string>      InputStateMap;

typedef ics::ArrayMap<std::string,InputStateMap>    FA;
typedef ics::pair<std::string,InputStateMap>        FAEntry;

bool gt_FAEntry (const FAEntry& a, const FAEntry& b)
{return a.first<b.first;}

typedef ics::ArrayPriorityQueue<FAEntry,gt_FAEntry> FAPQ;

typedef ics::pair<std::string,std::string>          Transition;
typedef ics::ArrayQueue<Transition>                 TransitionQueue;


//Read an open file describing the finite automaton (each line starts with
//  a state name followed by pairs of transitions from that state: (input
//  followed by new state, all separated by semicolons), and return a Map
//  whose keys are states and whose associated values are another Map with
//  each input in that state (keys) and the resulting state it leads to.
const FA read_fa(std::ifstream &file) {
      std::string line;
      FA fa;
      while(std::getline(file,line)) {
          ics::ArrayMap<std::string,std::string> temp1;
          std::vector<std::string> temp2=ics::split(line,";");
          fa.put(temp2[0],temp1);
          for(int i=1; i<temp2.size(); i++){
              if(i%2==1){
                  fa[temp2[0]].put(temp2[i],temp2[i+1]);
              }
          }
      }
      return fa;
}


//Print a label and all the entries in the finite automaton Map, in
//  alphabetical order of the states: each line has a state, the text
//  "transitions:" and the Map of its transitions.
bool b1(const ics::pair<std::string,ics::ArrayMap<std::string,std::string>>& a, const ics::pair<std::string,ics::ArrayMap<std::string,std::string>>& b)
{return a.first < b.first;}

void print_fa(const FA& fa) {
    std::cout << "The Finite Automaton's Description" << std::endl;
    ics::ArrayPriorityQueue<ics::pair<std::string,ics::ArrayMap<std::string,std::string>>,b1> temp;
    temp.enqueue_all(fa);
    for(ics::pair<std::string,ics::ArrayMap<std::string,std::string>>& q : temp){
        std::cout << "  " << q.first << " transitions: " << q.second << std::endl;
    }
}


//Return a queue of the calculated transition pairs, based on the finite
//  automaton, initial state, and queue of inputs; each pair in the returned
//  queue is of the form: input, new state.
//The first pair contains "" as the input and the initial state.
//If any input i is illegal (does not lead to a state in the finite
//  automaton), then the last pair in the returned queue is i,"None".
TransitionQueue process(const FA& fa, std::string state, const InputsQueue& inputs) {
    TransitionQueue t;
    t.enqueue(ics::make_pair(std::string(""),state));
    std::string tempstate=state;
    for(std::string& s : inputs){
        if (fa[tempstate].has_key(s)==0){
            t.enqueue(ics::make_pair(s,std::string("NONE")));
            break;
        }
        tempstate=fa[tempstate][s];
        t.enqueue(ics::make_pair(s,tempstate));
    }
    return t;
}


//Print a TransitionQueue (the result of calling the process function above)
// in a nice form.
//Print the Start state on the first line; then print each input and the
//  resulting new state (or "illegal input: terminated", if the state is
//  "None") indented on subsequent lines; on the last line, print the Stop
//  state (which may be "None").
void interpret(TransitionQueue& tq) {  //or TransitionQueue or TransitionQueue&&
    std::string state = tq.dequeue().second;
    std::cout << "Start state =  " << state << std::endl;
    for(ics::pair<std::string,std::string>& p : tq){
        if(p.second=="NONE"){
            std::cout << "  Input = " << p.first << "; illegal input: terminated" <<std::endl;
            state=p.second;
            break;
        }
        else{
            std::cout << "  Input = " << p.first << "; new state = "<< p.second <<std::endl;
            state=p.second;
        }
    }
    std::cout << "Stop state = " << state << std::endl;
}



//Prompt the user for a file, create a finite automaton Map, and print it.
//Prompt the user for a file containing any number of simulation descriptions
//  for the finite automaton to process, one description per line; each
//  description contains a start state followed by its inputs, all separated by
//  semicolons.
//Repeatedly read a description, print that description, put each input in a
//  Queue, process the Queue and print the results in a nice form.
int main() {
  try {
      std::string file1;
      std::string file2;
      std::cout << "Enter a finite automaton's file: " ;
      std::getline(std::cin,file1);
      std::ifstream g1(file1);
      FA fa=read_fa(g1);
      print_fa(fa);
      std::cout << std::endl;
      std::cout << "Enter a start-state and input file: " ;
      std::getline(std::cin,file2);
      std::cout << std::endl;
      std::ifstream g2(file2);
      std::string line;
      while(std::getline(g2,line)) {
          std::cout << "Starting a new simulation with description: " << line << std::endl;
          ics::ArrayMap<std::string, std::string> temp1;
          std::vector<std::string> temp2 = ics::split(line, ";");
          ics::ArrayQueue<std::string> instructs;
          std::string start=temp2[0];
          for(int i=1; i<temp2.size(); i++){
              instructs.enqueue(temp2[i]);
          }
          TransitionQueue t=process(fa,start,instructs);
          interpret(t);
          std::cout << std::endl;
      }

  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
