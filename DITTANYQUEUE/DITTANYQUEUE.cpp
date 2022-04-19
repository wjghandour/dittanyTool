/************************************************
 *      Copyright 2010	- 2022			  *
 *      Walid Jawad Ghandour                    *
 *                                              *
 *                                              *
 *      All Rights Reserved                     *
 ************************************************/


// This tool was builty and run using pin-2.12-58423-gcc.4.4.7-linux

#include "dittany.h"
#include "BasicBlock.H"
#include "RoutineGraph.H"


using std::setw;

#define IMPLICIT 

std::ofstream log, logValues, logCount;

static UINT64 icount = 0;
static UINT64  totalIns = 0;


/* ===================================================================== */
// Command line switches
/* ===================================================================== */

// knobs are used to pass inputs by the user
//all of them are optional

KNOB<UINT64>   KnobFlowTotal(KNOB_MODE_WRITEONCE,  "pintool",
    "total", "0", "upper number");

KNOB<BOOL>   KnobFlowLoadSrc(KNOB_MODE_WRITEONCE,  "pintool",
    "LoadSrc", "0", "Flows Into Instr with Load as Src");

KNOB<BOOL>   KnobLoadTarg(KNOB_MODE_WRITEONCE,  "pintool",
    "LoadTarg", "0", "Flows Into Loads");

KNOB<BOOL> KnobDataFlow(KNOB_MODE_WRITEONCE, "pintool", "d", "0",
"data flow analysis");

KNOB<BOOL> KnobControlFlow(KNOB_MODE_WRITEONCE, "pintool", "c", "0",
"control flow analysis");

KNOB<string> KnobSRTN(KNOB_MODE_WRITEONCE,  "pintool",
    "sRtn", "", "specify start rtn");

KNOB<string> KnobERTN(KNOB_MODE_WRITEONCE,  "pintool",
    "eRtn", "", "specify end  rtn");


KNOB<BOOL>   KnobSelRTN(KNOB_MODE_WRITEONCE,  "pintool",
    "s", "0", "specify start and end rtn");

KNOB<BOOL>   KnobUniqRTN(KNOB_MODE_WRITEONCE,  "pintool",
    "u", "0", "specify the rtn to instrument");


KNOB<UINT64>   KnobSkip(KNOB_MODE_WRITEONCE,  "pintool",
    "skip", "0", "skip specified number of instructions");

KNOB<BOOL>   KnobPath(KNOB_MODE_WRITEONCE,  "pintool",
    "path", "0", "specify a path");

KNOB<UINT64>   KnobStartAddr(KNOB_MODE_WRITEONCE,  "pintool",
    "sIP", "0", "start instructions");

KNOB<UINT64>   KnobEndAddr(KNOB_MODE_WRITEONCE,  "pintool",
    "eIP", "0", "end instructions");

// ================================================================

//var declaration
string startRtn;
string endRtn;
static UINT64 skipNum = 0;
static ADDRINT startIP = 0;
static ADDRINT endIP = 0;

static bool start_instrumentation = false;
static bool skip_instrumentation = true; 
bool calling_flag = false;

//Load source declarations
typedef std::set<ADDRINT> Load_set;
Load_set::iterator load_set_it;
Load_set ld_set;


typedef std::set<ADDRINT> ADDRINT_set;
ADDRINT_set::iterator addrint_set_it;

map<REG, ADDRINT_set > regDepMap;
map<ADDRINT, ADDRINT_set > memDepMap;
map<ADDRINT, ADDRINT_set > instrDepMap;
map<ADDRINT, ADDRINT> instrLVMap;
map<ADDRINT, ADDRINT> instrLVMapPre;
map<ADDRINT, bool> instrLVMapF;
map<ADDRINT, ADDRINT_set > instrTmpDepMap;

map<ADDRINT, ADDRINT_set > controlDepMap;
stack<ADDRINT> controlDepStack;
map<ADDRINT, ADDRINT> BrIpdomMap;
map<ADDRINT, ADDRINT_set> controls;

ADDRINT_set dest_set;
ADDRINT_set src_set;
ADDRINT_set base;
ADDRINT_set idx;
ADDRINT_set eflags;
ADDRINT_set eax;
ADDRINT_set edx;
ADDRINT_set cnt;
ADDRINT_set esp;

std::set<INT64> indexSet;
std::set<INT64>::iterator it_indexSet;

// This function is called before every instruction is executed
VOID docount() { 
  icount++;

  if ((totalIns > 0) && (totalIns < icount)){
    PIN_ExitApplication(1) ;
  }
}

//ValuePairs class is used to store pairs of values of source and target instructions.
class ValuePairs{

public:
  ValuePairs();
  ValuePairs(ADDRINT, ADDRINT);
  void Initialize(ADDRINT, ADDRINT);
  bool sortValuePairs(ValuePairs);
  void printValuePairs();

private:
  //two private data members of type ADDRINT that are used to hold the
  //values of the source and of the target instructions.
  INT32  v1, v2;
};

ValuePairs::ValuePairs(){
  v1 = v2 = 0;
}

ValuePairs::ValuePairs(ADDRINT a, ADDRINT b){
  this->v1 = a;
  this->v2 = b;
}

// Used to set the values of the data members.

void ValuePairs::Initialize(ADDRINT a, ADDRINT b){
  this->v1 = a;
  this->v2 = b;
}


//Used to print the data members values.
void ValuePairs::printValuePairs(){

  logValues<<"  "<<v1<<"  "<<v2<<" "; 
}


//Used to sort ValuePairs.
bool ValuePairs::sortValuePairs(ValuePairs k){
  if (this->v1 < k.v1){
    return true;
  }
  else if ((this->v1 == k.v1) && (this->v2 < k.v2)){
    return true;
  }
  else 
    return false;
}

bool operator<(ValuePairs a, ValuePairs b)
{
  return  a.sortValuePairs(b);
}

typedef std::map<ValuePairs, int> ValuePairsIntMap;
ValuePairsIntMap::const_iterator ValuePairsIntMap_iter;
typedef std::map<ValuePairs, int>::value_type ValuePairsIntMap_type;

typedef std::queue<ValuePairs> ValuePairsQueue;

//class that we use to store dependences between instructions
class Depend{

public:
  Depend();
  Depend(ADDRINT, ADDRINT);
  void setDepend(ADDRINT, ADDRINT);
  void setValues(ADDRINT, ADDRINT);
  void printValues();
  bool sortDepend(Depend);
  bool equalsDepend(Depend);

private:

  // targ_inst and src_inst  satore the address of the source and target instructions.
  ADDRINT targ_inst;
  ADDRINT src_inst;

  //        A data member of type (ValuePairs, int) map. It is used to store a
  //pair of values taken by the source and target instructions and the number
  //of occurrences of this pair.

  ValuePairsIntMap values;

  //     The number of occurrences of the dependence between the source and
  //target instructions.

  ValuePairsQueue valuesQueue;

  int size;


};

Depend::Depend(){
  this->targ_inst = 0;
  this->src_inst = 0;
  this->size = 0;
}

Depend::Depend(ADDRINT i1, ADDRINT i2){
  this->targ_inst = i1;
  this->src_inst = i2;
  this->size=0;
}

void Depend::setDepend(ADDRINT i1, ADDRINT i2){
  this->targ_inst = i1;
  this->src_inst = i2;
  this->size = 0;
}

bool Depend::sortDepend(Depend d1){

  if (this->src_inst < d1.src_inst){
    return true;
  }
  else 
    return false; 
}

bool Depend::equalsDepend(Depend d1){

  if (this->src_inst == d1.src_inst){  
    return true;
  }
  else 
    return false; 
}

// Add a new pair of values of the source
//and target instructions. It increments the size data member
void Depend::setValues(ADDRINT v1, ADDRINT v2){

  if (this->size < 50){

  this->size++;

  ValuePairs vp;
  vp.Initialize((INT32) v1, (INT32) v2);

  if ( values.count(vp) > 0){

    values[vp] = values[vp] +1;
  }
  else {
    values.insert(ValuePairsIntMap_type(vp,1));
  }

  valuesQueue.push(vp);

  }
}

void Depend::printValues(){

  if (size > 3){

    logValues<<src_inst<<"  "<<targ_inst<<" "<<size<<" ";


  while (!valuesQueue.empty())
  {
     ValuePairs vp = (ValuePairs) valuesQueue.front();
      vp.printValuePairs();
      valuesQueue.pop();
  }

   logValues<<endl;
 
 
  }

}

bool operator<(Depend a, Depend b)
{
  return  a.sortDepend(b);
}

bool operator==(Depend a, Depend b)
{
  return  a.equalsDepend(b);
}

typedef std::set<Depend> Depend_set;
Depend_set::iterator Depend_set_it;

//InsBufferEntry class holds all information required about an instruction, such as:
//instruction address, thread ID, routine name, source and destination operands
//type and values. It has several data members and functions to store and get this
//information. It also contains a self pointer InsBufferEntry *next and a set of type
//Depend, insrDepSet, that holds the information about the instructions on which
//the corresponding instruction depends.

class InsBufferEntry {

public:

  InsBufferEntry();
 

  void SetInsAddr(ADDRINT);
  ADDRINT GetInsAddr();
  void PrintInsAddr();
  void SetTid(THREADID);
  THREADID GetTid();
  void SetRtnName(string);
  string GetRtnName();

  void SetSourceRegID(int);
  void SetDestRegID(int);
  int GetSourceRegID();
  int GetDestRegID();
  void SetSourceRegName(string);
  void SetDestRegName(string);
  string GetSourceRegName();
  string GetDestRegName();
  void PrintSourceRegID();
  void PrintDestRegID();

  void SetSourceMemAdd(ADDRINT);
  void SetSourceMemAddSize(ADDRINT);
  void SetSourceMemDisp(INT64);
  void SetSourceMemScale(UINT32);
  INT64 GetSourceMemDisp();
  UINT32 GetSourceMemScale();
  ADDRINT GetSourceMemAdd();
  ADDRINT GetSourceMemAddSize();
  void SetDestMemAdd(ADDRINT);  
  void SetDestMemAddSize(ADDRINT);
  void SetDestMemDisp(INT64);
  void SetDestMemScale(UINT32);
  INT64 GetDestMemDisp();
  UINT32 GetDestMemScale();
  ADDRINT GetDestMemAdd();
  ADDRINT GetDestMemAddSize();
  void PrintSourceMemAdd();
  void PrintDestMemAdd();

  void SetSourceRegFlag();
  bool GetSourceRegFlag();

  void SetDestRegFlag();
  bool GetDestRegFlag();

  void SetSourceMemFlag();
  bool GetSourceMemFlag();

  void SetDestMemFlag();
  bool GetDestMemFlag();

  void SetSourceImmFlag();
  bool GetSourceImmFlag();

  void SetDestImmFlag();
  bool GetDestImmFlag();

  void SetSourceMemIndexReg(string);
  void SetSourceMemBaseReg(string);
  void SetDestMemIndexReg(string);
  void SetDestMemBaseReg(string);
  string GetSourceMemIndexReg();
  string GetSourceMemBaseReg();
  string GetDestMemIndexReg();
  string GetDestMemBaseReg();

  void StoreDestRegVal(ADDRINT);
  void StoreOperand0RegVal(ADDRINT);
  void StoreOperand1RegVal(ADDRINT);

  bool GetDestMemAddFlag();
  void SetDestMemAddFlag();

  bool GetSrcMemAddFlag();
  void SetSrcMemAddFlag();

  void SetInstruction(string);
  string GetInstruction();



  bool FindDepend(Depend);
  void AddDepend(Depend);
  void UpdateDepend(Depend, ADDRINT, ADDRINT);
  void PrintValues();
  void SetNextPtr(InsBufferEntry *);
  InsBufferEntry *GetNextPtr();


private:

  int s0, s1, dest;
  ADDRINT insAddr;
  THREADID tid;

  string instruction;
  string rtn_name; 
  string source_reg_name;
  string dest_reg_name;
  string reg_name;

  int source_reg_id;
  int dest_reg_id;

 
  INT64 src_oper_mem_displacement;
  UINT32 src_oper_memory_scale;

  string src_oper_mem_base_reg;
  string src_oper_mem_index_reg;
 
  INT64 dest_oper_mem_displacement;
  UINT32 dest_oper_memory_scale;

  string dest_oper_mem_base_reg;
  string dest_oper_mem_index_reg;

  ADDRINT source_value;
  ADDRINT dest_value;

 
  bool src_rip_flag;
  bool dest_rip_flag;

  bool src_mem_flag;
  bool dest_mem_flag;
  bool src_reg_flag;
  bool dest_reg_flag;
  bool src_imm_flag;
  bool dest_imm_flag;
  bool dest_mem_add_flag;
  bool src_mem_add_flag;

  ADDRINT src_mem_add;
  ADDRINT src_mem_add_size;
  ADDRINT dest_mem_add;
  ADDRINT dest_mem_add_size;


  ADDRINT  Source1RegValArray[100];  
  ADDRINT  Source0RegValArray[100];
  ADDRINT  DestRegValArray[100];


  int dest_mem_add_count;
  int src_mem_add_count;
  int inst_counter;

  vector<ADDRINT> instDependences;

  Depend_set insrDepSet;

  InsBufferEntry *next;
};


InsBufferEntry::InsBufferEntry(){

s0 = s1 = dest = insAddr = tid = 0;

 source_reg_id = dest_reg_id = 0;
rtn_name =  source_reg_name =   dest_reg_name =  reg_name =  "";

src_oper_mem_base_reg =  src_oper_mem_index_reg = dest_oper_mem_base_reg = dest_oper_mem_index_reg = "";
 
src_oper_mem_displacement = src_oper_memory_scale = dest_oper_mem_displacement = dest_oper_memory_scale =  source_value = dest_value = 0; 

src_rip_flag = dest_rip_flag = src_mem_flag = dest_mem_flag = src_reg_flag =    dest_reg_flag =  src_imm_flag = dest_imm_flag = dest_mem_add_flag = false;

src_mem_add = src_mem_add_size = dest_mem_add = 0;


 dest_mem_add_count =  src_mem_add_count = inst_counter = 0;
}


void InsBufferEntry::SetSourceRegID(int r){
  this->source_reg_id = r;
}

void InsBufferEntry::SetDestRegID(int r){

  this->dest_reg_id = r;
}

int InsBufferEntry::GetSourceRegID(){
  return this->source_reg_id;
}

int InsBufferEntry::GetDestRegID(){
  return this->dest_reg_id;
}

void InsBufferEntry::SetSourceRegName(string r){
  this->source_reg_name = r;
}

void InsBufferEntry::SetDestRegName(string r){
  this->dest_reg_name = r;
}

string InsBufferEntry::GetSourceRegName(){
  return this->source_reg_name;
}

string InsBufferEntry::GetDestRegName(){
  return this->dest_reg_name;
}

void InsBufferEntry::PrintSourceRegID(){
  cout<<"source reg id: "<<this->source_reg_id<<endl;
}

void InsBufferEntry::PrintDestRegID(){
  cout<<"dest reg id: "<<this->dest_reg_id<<endl;
}

void InsBufferEntry::SetSourceMemAdd(ADDRINT r){
  if (this->src_mem_add != r){
    this->src_mem_add = r;
    this->src_mem_add_flag = false; 
  }
  else {
     this->src_mem_add_flag = true; 
  }
}

void InsBufferEntry::SetSourceMemAddSize(ADDRINT r){
  this->src_mem_add_size = r;
}

ADDRINT InsBufferEntry::GetSourceMemAdd(){
  return this->src_mem_add;
}

ADDRINT InsBufferEntry::GetSourceMemAddSize(){
  return this->src_mem_add_size;
}

void InsBufferEntry::SetSourceMemDisp(INT64 mem_disp){
  this->src_oper_mem_displacement  = mem_disp;
}

void InsBufferEntry::SetSourceMemScale(UINT32 mem_scale){
  this->src_oper_memory_scale = mem_scale;
}

INT64 InsBufferEntry::GetSourceMemDisp(){
  return this->src_oper_mem_displacement;
}

UINT32 InsBufferEntry::GetSourceMemScale(){
  return this->src_oper_memory_scale;
}

bool InsBufferEntry::GetSrcMemAddFlag(){
  return this->src_mem_add_flag;
}

void InsBufferEntry::SetSrcMemAddFlag(){
  this->src_mem_add_flag = true;
}

void InsBufferEntry::SetDestMemAdd(ADDRINT r){
  if (this->dest_mem_add != r){
    this->dest_mem_add = r;
    this->dest_mem_add_flag = false; 
  }
  else {
     this->dest_mem_add_flag = true; 
  }
}

bool InsBufferEntry::GetDestMemAddFlag(){
  return this->dest_mem_add_flag;
}

void InsBufferEntry::SetDestMemAddFlag(){
  this->dest_mem_add_flag = true;
}

void InsBufferEntry::SetDestMemAddSize(ADDRINT r){
  this->dest_mem_add_size = r;
}

void InsBufferEntry::SetDestMemDisp(INT64 mem_disp){
  this->dest_oper_mem_displacement  = mem_disp;
}

void InsBufferEntry::SetDestMemScale(UINT32 mem_scale){
  this->dest_oper_memory_scale  = mem_scale;
}

INT64 InsBufferEntry::GetDestMemDisp(){
  return this->dest_oper_mem_displacement;
}

UINT32 InsBufferEntry::GetDestMemScale(){
  return this->dest_oper_memory_scale;
}

ADDRINT InsBufferEntry::GetDestMemAdd(){
  return this->dest_mem_add;
}

ADDRINT InsBufferEntry::GetDestMemAddSize(){
  return this->dest_mem_add_size;
}
void InsBufferEntry::PrintSourceMemAdd(){
  cout<<"source mem add: "<<this->src_mem_add<<endl;
}

void InsBufferEntry::PrintDestMemAdd(){
  cout<<"dest mem add: "<<this->dest_mem_add<<endl;
}

void InsBufferEntry::SetInsAddr(ADDRINT addr){
  this->insAddr = addr;
}

ADDRINT InsBufferEntry::GetInsAddr(){
  return this->insAddr;
}

void InsBufferEntry::SetTid(THREADID tid){
  this->tid = tid;
}

THREADID InsBufferEntry::GetTid(){
  return this->tid;
}

void InsBufferEntry::SetRtnName(string rtn){
  this->rtn_name = rtn;
}

string InsBufferEntry::GetRtnName(){
  return this->rtn_name;
}


void InsBufferEntry::PrintInsAddr(){
  cout<<"ins addr: "<<this->insAddr<<endl;
}

void InsBufferEntry::SetSourceRegFlag(){
  this->src_reg_flag = true;
}

bool InsBufferEntry::GetSourceRegFlag(){
  return this->src_reg_flag;
}

void InsBufferEntry::SetDestRegFlag(){
  this->dest_reg_flag = true;
}

bool InsBufferEntry::GetDestRegFlag(){
  return this->dest_reg_flag;
}

void InsBufferEntry::SetSourceMemFlag(){
  this->src_mem_flag = true;
}

bool InsBufferEntry::GetSourceMemFlag(){
  return this->src_mem_flag;
}

void InsBufferEntry::SetDestMemFlag(){
  this->dest_mem_flag = true;
}

bool InsBufferEntry::GetDestMemFlag(){
  return this->dest_mem_flag;
}

void InsBufferEntry::SetSourceImmFlag(){
  this->src_imm_flag = true;
}

bool InsBufferEntry::GetSourceImmFlag(){
  return this->src_imm_flag;
}

void InsBufferEntry::SetDestImmFlag(){
  this->dest_imm_flag = true;
}

bool InsBufferEntry::GetDestImmFlag(){
  return this->dest_imm_flag;
}

void InsBufferEntry::SetSourceMemIndexReg(string  mem_index){
  this->src_oper_mem_index_reg = mem_index; 
}

void InsBufferEntry::SetSourceMemBaseReg(string  mem_base){
  this->src_oper_mem_base_reg = mem_base; 
}

void InsBufferEntry::SetDestMemIndexReg(string  mem_index){
  this->dest_oper_mem_index_reg = mem_index; 
}

void InsBufferEntry::SetDestMemBaseReg(string  mem_base){
  this->dest_oper_mem_base_reg = mem_base; 
}

string InsBufferEntry::GetSourceMemIndexReg(){
  return this->src_oper_mem_index_reg; 
}

string InsBufferEntry::GetSourceMemBaseReg(){
  return this->src_oper_mem_base_reg; 
}

string InsBufferEntry::GetDestMemIndexReg(){
  return this->dest_oper_mem_index_reg; 
}

string InsBufferEntry::GetDestMemBaseReg(){
  return this->dest_oper_mem_base_reg; 
}




void InsBufferEntry::SetInstruction(string ins){
  this->instruction = ins;
}

string InsBufferEntry::GetInstruction(){
  return this->instruction;
}




bool InsBufferEntry::FindDepend(Depend d){
 if (insrDepSet.find(d) == insrDepSet.end())
    return false;
  else return true;
}

void InsBufferEntry::AddDepend(Depend d){
 if (insrDepSet.find(d) == insrDepSet.end()){
    insrDepSet.insert(d);
  }
  else {
    cout<<"it exists"<<endl;
  }
}

//          It takes three arguments: the first one of type Depend, the second and
//the third of type ADDRINT. The function first check if the dependence passed
//using its first argument exists in its dependences set. If found, we only update it
//with the pair of values passed using the second and third arguments. If it is not
//found, we insert it and assign to it the pair of values passed.

void InsBufferEntry::UpdateDepend(Depend d, ADDRINT targ_value, ADDRINT src_value){ 
  if (insrDepSet.find(d) == insrDepSet.end()){ 
    //dependence not found, insert it
    d.setValues(src_value,targ_value);
     insrDepSet.insert(d);
   }
   else {
     //dependence found, update it with the pair of values
     Depend_set_it =  insrDepSet.find(d);
     Depend tmp = *Depend_set_it;
     tmp.setValues(src_value,targ_value);
     insrDepSet.erase(d);
     insrDepSet.insert(tmp);

   }
}

void InsBufferEntry::SetNextPtr(InsBufferEntry *n){
  this->next = n;
}

InsBufferEntry* InsBufferEntry::GetNextPtr(){
  return this->next;
}

void InsBufferEntry::PrintValues(){



  for (Depend_set_it=insrDepSet.begin(); Depend_set_it!=insrDepSet.end(); Depend_set_it++){
    ((Depend) (*Depend_set_it)).printValues();
  }

  if (this->next != NULL){
    (this->next)->PrintValues();
  }

}

//check if the instruction was encoutered
//before, otherwise, insert a new one

InsBufferEntry* insBuffer[10000];

//check if the instruction was previously
//encountered
//if not allocate an entry for it
//for distinct instructions with the same index
//the old one points to the new one
InsBufferEntry* findInsBufferEntry(ADDRINT a){

  InsBufferEntry* k;

  INT64 index = (INT64)(a % 10000);

  indexSet.insert(index);

  k = insBuffer[index];

  if(k != NULL){

	   while(k != NULL){

	      if(k->GetInsAddr() == a){

				break;
	      }
	      
	      k = k->GetNextPtr();
	   }


	   if(k == NULL){


		  k = new InsBufferEntry();
		  k->SetInsAddr(a);

		  InsBufferEntry* tmp = insBuffer[index];
		  while(tmp->GetNextPtr() != NULL)
				tmp = tmp->GetNextPtr();
		  tmp->SetNextPtr(k);
	   }
	}
	else{

	   k = new InsBufferEntry();
	   k->SetInsAddr(a);

	   insBuffer[index] = k;
       	}

	return k;
}



typedef void (*InstrumentFunction)(INS ins, void *v);

InstrumentFunction instrument_functions[XED_ICLASS_LAST];

#include "instrument_functions.c"

//calculate the ipdoms of the branches
static void Controlflow(RTN rtn, void *v)
{
  if( !RTN_Valid(rtn))
		  return;

  string rtn_name = RTN_Name(rtn);

  IMG img = SEC_Img(RTN_Sec(rtn));

  if(LEVEL_CORE::IMG_TYPE_SHAREDLIB == IMG_Type(img)) return;


  RTN_Open(rtn);

  RoutineGraph *rtnGraph = new RoutineGraph(rtn);


  for(INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins)) {

    if(XED_CATEGORY_COND_BR == INS_Category(ins)) {

      ADDRINT addr = INS_Address(ins);
      BasicBlock *block = rtnGraph->addressMap[addr];
      if(NULL == block) {

	continue;

      }

      BasicBlock *ipdomBlock = block->getPostDominator();
      if(NULL == ipdomBlock) {

	continue;
      }
      ADDRINT ipdomAddr = ipdomBlock->startingAddress;
   

      if(controls.find(ipdomAddr) == controls.end()) {
	controls[ipdomAddr] = set<ADDRINT>();
      }

      //ipdom
      controls[ipdomAddr].insert(addr);

      //set the ipdom of  a branch
      BrIpdomMap[addr] = ipdomAddr;

    }

  }
  
 
  
  delete rtnGraph;

  RTN_Close(rtn);
}


//instrument instructions
//take command line options into consideration
//call the appropriate type-aware function
VOID my_instrument_inst(INS ins, InsBufferEntry* e){

  xed_iclass_enum_t opcode = (xed_iclass_enum_t) INS_Opcode(ins);


  string rtn_name = RTN_Name(INS_Rtn(ins));

  IMG img = SEC_Img(RTN_Sec(INS_Rtn(ins)));

  if(LEVEL_CORE::IMG_TYPE_SHAREDLIB == IMG_Type(img)) return;


  //command line options

  if (KnobUniqRTN){
	    if (! rtn_name.compare(KnobSRTN.Value())){
	      start_instrumentation = true;	 
	    }
	    else {
	      start_instrumentation = false;
	    }
  }

  else if (KnobSelRTN){
	    if (! rtn_name.compare(KnobSRTN.Value())){
	      start_instrumentation = true;	 
	    }
	    else if (rtn_name == endRtn){
	      start_instrumentation = false;
	    }
  }
  else {
          start_instrumentation = true;
  }
 
  if ((skipNum > 0) && (skipNum > icount)) {
    skip_instrumentation = false;
  }
  else if (skipNum > 0){
    skip_instrumentation = true;
  }

  e->SetInstruction(INS_Disassemble(ins));


  if (start_instrumentation && skip_instrumentation){
   (*instrument_functions[opcode])(ins, e);
  }

  if (start_instrumentation && skip_instrumentation && KnobControlFlow){
    //branch: get its dep and push it in the stack
    if(XED_CATEGORY_COND_BR == INS_Category(ins)) {
      if ( BrIpdomMap.find(INS_Address(ins)) != BrIpdomMap.end()){

		INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CONDBr),
		       IARG_ADDRINT, INS_Address(ins),
		   IARG_END);

      }
      
    }

    //ipdom, call pop funtion
    if (controls.find(INS_Address(ins)) != controls.end()) {

      	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(POPCONDBr),
		       IARG_ADDRINT, INS_Address(ins),
		   IARG_END);
    }
  }

}


// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{

  if( !RTN_Valid(INS_Rtn(ins)))
		  return;

  string rtn_name = RTN_Name(INS_Rtn(ins));

  IMG img = SEC_Img(RTN_Sec(INS_Rtn(ins)));

  if(LEVEL_CORE::IMG_TYPE_SHAREDLIB == IMG_Type(img)) return;


  PIN_SetSyntaxATT();
 

  ADDRINT insAddr = INS_Address(ins);

 InsBufferEntry* e = findInsBufferEntry(insAddr);


 if (start_instrumentation){

     INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);

 }

 //store load instructions when LoadSource flag is set
 if ((KnobFlowLoadSrc || KnobLoadTarg)   &&  start_instrumentation  && INS_IsMov(ins) &&  INS_OperandIsMemory(ins,1) ){
         ld_set.insert(insAddr);
    }

 my_instrument_inst(ins, e);

}


// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
 
  logCount<<icount<<endl;

  for (it_indexSet=indexSet.begin(); it_indexSet!=indexSet.end(); it_indexSet++){
   ((InsBufferEntry*) insBuffer[*it_indexSet])->PrintValues();
  }

  logValues.flush();
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char * argv[])
{

  regDepMap.clear();
  memDepMap.clear();
  instrDepMap.clear();

  log.open("out.log");
  logValues.open("valuesSlicing.log");
  logCount.open("count.log");

    // Initialize pin
    PIN_Init(argc, argv);

    PIN_InitSymbols() ;

    startRtn = KnobSRTN.Value();
    endRtn = KnobERTN.Value();
    totalIns = KnobFlowTotal.Value();

    skipNum = KnobSkip.Value();

    if (KnobPath){

      startIP = KnobStartAddr.Value();

      //cout<<"startIP: "<<startIP<<endl;

      endIP =   KnobEndAddr.Value();

      //cout<<"endIP: "<<endIP<<endl;

    }


    // Register Instruction to be called to instrument instructions
    if (KnobDataFlow){
      INS_AddInstrumentFunction(Instruction, 0);
    }

    if (KnobControlFlow){
      RTN_AddInstrumentFunction(Controlflow, 0);
    }

    for(int i = 0; i < XED_ICLASS_LAST; i++) {
        instrument_functions[i] = &UnimplementedInstruction;
    }


    instrument_functions[XED_ICLASS_ADC] = &Instrument_ADC; // 6
    instrument_functions[XED_ICLASS_ADD] = &Instrument_ADD; // 1
    instrument_functions[XED_ICLASS_AND] = &Instrument_AND; // 8
    instrument_functions[XED_ICLASS_CDQ] = &Instrument_CDQ; //70
    instrument_functions[XED_ICLASS_CLD] = &Instrument_CLD; //140
    
   


    instrument_functions[XED_ICLASS_CMPSB] = &Instrument_CMPSB; //85
    instrument_functions[XED_ICLASS_CMPXCHG] = &Instrument_CMPXCHG; //231

    instrument_functions[XED_ICLASS_CWDE] = &Instrument_CWDE; //67
    instrument_functions[XED_ICLASS_DEC] = &Instrument_DEC; // 22

    instrument_functions[XED_ICLASS_DIV] = &Instrument_DIV; //482
    instrument_functions[XED_ICLASS_IDIV] = &Instrument_IDIV; //483

    instrument_functions[XED_ICLASS_MUL] = &Instrument_MUL; //481
    instrument_functions[XED_ICLASS_IMUL] = &Instrument_IMUL; // 35

    instrument_functions[XED_ICLASS_LEA] = &Instrument_LEA; //62

    instrument_functions[XED_ICLASS_MOV] = &Instrument_MOV; //61

    instrument_functions[XED_ICLASS_MOVSB] = &Instrument_MOVSB; //81
    instrument_functions[XED_ICLASS_MOVSW] = &Instrument_MOVSW; //82
    instrument_functions[XED_ICLASS_MOVSD] = &Instrument_MOVSD; //83
    instrument_functions[XED_ICLASS_MOVSX] = &Instrument_MOVSX; //378
    instrument_functions[XED_ICLASS_MOVZX] = &Instrument_MOVZX; //236

    instrument_functions[XED_ICLASS_NEG] = &Instrument_NEG; //480
    
    instrument_functions[XED_ICLASS_NOT] = &Instrument_NOT; //479
    instrument_functions[XED_ICLASS_OR] = &Instrument_OR; // 4
    instrument_functions[XED_ICLASS_PAUSE] = &Instrument_PAUSE; //64

    instrument_functions[XED_ICLASS_NOP] = &Instrument_NOP;

    instrument_functions[XED_ICLASS_POP] = &Instrument_POP; // 3
    instrument_functions[XED_ICLASS_POPFD] = &Instrument_POPFD; //77
    instrument_functions[XED_ICLASS_PUSH] = &Instrument_PUSH; // 2
    instrument_functions[XED_ICLASS_PUSHFD] = &Instrument_PUSHFD; //74

   
    instrument_functions[XED_ICLASS_RDTSC] = &Instrument_RDTSC; //169


    instrument_functions[XED_ICLASS_SAR] = &Instrument_SAR; //478
    instrument_functions[XED_ICLASS_SBB] = &Instrument_SBB; // 7
    instrument_functions[XED_ICLASS_SCASB] = &Instrument_SCASB; //97

    instrument_functions[XED_ICLASS_SETB] = &Instrument_SETcc; //222
    instrument_functions[XED_ICLASS_SETNB] = &Instrument_SETcc; //223
    instrument_functions[XED_ICLASS_SETZ] = &Instrument_SETcc; //224
    instrument_functions[XED_ICLASS_SETNZ] = &Instrument_SETcc; //225
    instrument_functions[XED_ICLASS_SETBE] = &Instrument_SETcc; //226
    instrument_functions[XED_ICLASS_SETNBE] = &Instrument_SETcc; //227

    instrument_functions[XED_ICLASS_SHLD] = &Instrument_SHLD; //230
    instrument_functions[XED_ICLASS_SHL] = &Instrument_SHL; //476
    instrument_functions[XED_ICLASS_SHRD] = &Instrument_SHRD; //371
    instrument_functions[XED_ICLASS_SHR] = &Instrument_SHR; //477
    instrument_functions[XED_ICLASS_STD] = &Instrument_STD; //141
    instrument_functions[XED_ICLASS_STD] = &Instrument_STD; //141
    instrument_functions[XED_ICLASS_STMXCSR] = &Instrument_STMXCSR; //508
    instrument_functions[XED_ICLASS_STOSB] = &Instrument_STOSB; //89
    instrument_functions[XED_ICLASS_STOSD] = &Instrument_STOSD; //91
    instrument_functions[XED_ICLASS_SUB] = &Instrument_SUB; // 12
    instrument_functions[XED_ICLASS_TEST] = &Instrument_TEST; //59
    instrument_functions[XED_ICLASS_XADD] = &Instrument_XADD; //237

instrument_functions[XED_ICLASS_XCHG] = &Instrument_XCHG; //60
    instrument_functions[XED_ICLASS_XOR] = &Instrument_XOR; // 15

instrument_functions[XED_ICLASS_FNSTSW] = &Instrument_FNSTSW; //587


    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
