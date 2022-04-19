/************************************************
 *      Copyright 2010	- 2022			  *
 *      Walid Jawad Ghandour                    *
 *                                              *
 *                                              *
 *      All Rights Reserved                     *
 ************************************************/
 
 
// Bitmasks for accessing specific parts of eflags
#define CF_MASK 1 << 1 // bit 1
#define PF_MASK 1 << 2 // bit 2
#define AF_MASK 1 << 4 // bit 4
#define ZF_MASK 1 << 6 // bit 6
#define SF_MASK 1 << 7 // bit 7
#define OF_MASK 1 << 11 // bit 11

static VOID * WriteAddr;
static INT32 WriteSize;

static void Instrument_MUL(INS, void *);

//store addr and size
static VOID RecordWriteAddrSize(VOID * addr, INT32 size)
{
    WriteAddr = addr;
    WriteSize = size;
}


//store immediate value
//in these functions we store the pairs
//of values taken by the targ instrunction
//and its source instructions
//that were encoutered over the path 
//that resulted in the execution of the targ instr
VOID StoreSrcImmVal(ADDRINT inst_addr, UINT32 v1, VOID* _e) {

  InsBufferEntry* e = (InsBufferEntry*)_e;

  instrLVMap[inst_addr] = (INT32) v1;

  ADDRINT_set tmp = instrTmpDepMap[inst_addr];

  if (tmp.size() > 0){

    Depend dp;

    for (addrint_set_it=tmp.begin();addrint_set_it!=tmp.end();addrint_set_it++){

      dp.setDepend(inst_addr, *addrint_set_it);


      if (KnobPath){
	if ((*addrint_set_it == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(*addrint_set_it) != ld_set.end()){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else{
	e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);
      }

    }

  }

}



//store destination reg val

VOID StoreDestRegVal(ADDRINT inst_addr, ADDRINT v1, VOID* _e) {

  InsBufferEntry* e = (InsBufferEntry*)_e;

 
  instrLVMap[inst_addr] = (INT32) v1;

  ADDRINT_set tmp = instrTmpDepMap[inst_addr];



  if (tmp.size() > 0){

    Depend dp;

    for (addrint_set_it=tmp.begin();addrint_set_it!=tmp.end();addrint_set_it++){

      dp.setDepend(inst_addr, *addrint_set_it);

      if (KnobPath){
	if ((*addrint_set_it == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(*addrint_set_it) != ld_set.end()){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
     else {
      e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);
     }


    }

  }

}



VOID StoreDestRegVali(ADDRINT inst_addr, ADDRINT v1, VOID* _e) {

  InsBufferEntry* e = (InsBufferEntry*)_e;

  instrLVMapPre[inst_addr] = instrLVMap[inst_addr];
 
  instrLVMap[inst_addr] = (INT32) v1;

  Depend dps;

  if (instrLVMapF[inst_addr]){
  dps.setDepend(inst_addr, inst_addr);

     if (KnobPath){
       if ((inst_addr == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dps, instrLVMap[inst_addr], instrLVMapPre[inst_addr]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(inst_addr) != ld_set.end()){
	  e->UpdateDepend(dps, instrLVMap[inst_addr], instrLVMapPre[inst_addr]);	  
	}
      }
     else {
      e->UpdateDepend(dps, instrLVMap[inst_addr], instrLVMapPre[inst_addr]);
     }

  }
  else {
    instrLVMapF[inst_addr] = true;
  }

  ADDRINT_set tmp = instrTmpDepMap[inst_addr];



  if (tmp.size() > 0){

    Depend dp;

    for (addrint_set_it=tmp.begin();addrint_set_it!=tmp.end();addrint_set_it++){

      dp.setDepend(inst_addr, *addrint_set_it);

      if (KnobPath){
	if ((*addrint_set_it == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(*addrint_set_it) != ld_set.end()){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
     else {
      e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);
     }


    }

  }

}




//store source mem value
VOID SrcMemVal(ADDRINT inst_addr, VOID * Addr, INT32 ReadSize, VOID* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;

  size_t size =  ReadSize;

    ADDRINT value;

    PIN_SafeCopy(&value, Addr, size);
 
    instrLVMap[inst_addr] = (INT32) value;

  ADDRINT_set tmp = instrTmpDepMap[inst_addr];


  if (tmp.size() > 0){

    Depend dp;

    for (addrint_set_it=tmp.begin();addrint_set_it!=tmp.end();addrint_set_it++){

      dp.setDepend(inst_addr, *addrint_set_it);


      if (KnobPath){
	if ((*addrint_set_it == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(*addrint_set_it) != ld_set.end()){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else {
	e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);
      }


    }

  }

}

//store destination mem value
VOID DestMemVal(ADDRINT inst_addr, VOID* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;

  size_t size =  WriteSize;

    ADDRINT value;
    PIN_SafeCopy(&value, WriteAddr, size);

    instrLVMap[inst_addr] = (INT32) value;

  ADDRINT_set tmp = instrTmpDepMap[inst_addr];


  if (tmp.size() > 0){

    Depend dp;

    for (addrint_set_it=tmp.begin();addrint_set_it!=tmp.end();addrint_set_it++){

      dp.setDepend(inst_addr, *addrint_set_it);

      if (KnobPath){
	if ((*addrint_set_it == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(*addrint_set_it) != ld_set.end()){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
     else {
      e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);
     }


    }

  }

}



VOID DestMemVali(ADDRINT inst_addr, VOID* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;

  size_t size =  WriteSize;

 instrLVMapPre[inst_addr] = instrLVMap[inst_addr];
 


    ADDRINT value;
    PIN_SafeCopy(&value, WriteAddr, size);

    instrLVMap[inst_addr] = (INT32) value;


 Depend dps;

  if (instrLVMapF[inst_addr]){
  dps.setDepend(inst_addr, inst_addr);

     if (KnobPath){
       if ((inst_addr == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dps, instrLVMap[inst_addr], instrLVMapPre[inst_addr]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(inst_addr) != ld_set.end()){
	  e->UpdateDepend(dps, instrLVMap[inst_addr], instrLVMapPre[inst_addr]);	  
	}
      }
     else {
      e->UpdateDepend(dps, instrLVMap[inst_addr], instrLVMapPre[inst_addr]);
     }

  }
  else {
    instrLVMapF[inst_addr] = true;
  }

  ADDRINT_set tmp = instrTmpDepMap[inst_addr];


  if (tmp.size() > 0){

    Depend dp;

    for (addrint_set_it=tmp.begin();addrint_set_it!=tmp.end();addrint_set_it++){

      dp.setDepend(inst_addr, *addrint_set_it);

      if (KnobPath){
	if ((*addrint_set_it == startIP ) && (inst_addr == endIP )){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
      else if (KnobFlowLoadSrc){
	if (ld_set.find(*addrint_set_it) != ld_set.end()){
	  e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);	  
	}
      }
     else {
      e->UpdateDepend(dp, instrLVMap[inst_addr], instrLVMap[*addrint_set_it]);
     }


    }

  }

}




//methods to operate on sets of type  ADDRINT
VOID set_set_entries( ADDRINT_set &destSet,  ADDRINT_set srcSet){

     destSet.clear();

    for (addrint_set_it=srcSet.begin();addrint_set_it!=srcSet.end();addrint_set_it++){
      destSet.insert(*addrint_set_it);
    }
}

VOID set_union(ADDRINT_set &destSet,  ADDRINT_set srcSet){

  for (addrint_set_it=srcSet.begin();addrint_set_it!=srcSet.end();addrint_set_it++){
    destSet.insert(*addrint_set_it);
  }
}

//getting the dep of a register

void DepForRegister(REG reg, ADDRINT_set &tmpSet)
{

  tmpSet.clear();

  map<REG, ADDRINT_set>::iterator iter = regDepMap.find(reg);
  if(regDepMap.end() != iter) {
     set_set_entries(tmpSet, iter->second);
  }
}

//getting the dep of mem

void DepForMemory(ADDRINT start, ADDRINT size,
		    REG baseReg, REG indexReg, ADDRINT_set &tmpSet)
{
   tmpSet.clear();
 
   for(ADDRINT addr = start; addr < start + size; addr++) {
     map<ADDRINT, ADDRINT_set >::iterator iter = memDepMap.find(addr);
     if(memDepMap.end() != iter) {
      set_union(tmpSet, iter->second);  
     }
   }

#ifdef IMPLICIT
  if(REG_valid(baseReg)) {
    map<REG, ADDRINT_set>::iterator iter = regDepMap.find(baseReg);
    if(regDepMap.end() != iter) {
      set_union(tmpSet, iter->second);
    }
  }

  if(REG_valid(indexReg)) {
    map<REG, ADDRINT_set>::iterator iter = regDepMap.find(indexReg);
    if(regDepMap.end() != iter) {
      set_union(tmpSet, iter->second);
    }
 }
#endif

}

//setting the dep of reg

void SetDepForRegister(REG dest, ADDRINT inst_addr, bool set_instr,  int numOfArgs, ADDRINT_set &arg1, ADDRINT_set &arg2, ADDRINT_set &arg3)
{
  
 
  if(LEVEL_BASE::REG_ESP == dest ||
     LEVEL_BASE::REG_EBP == dest) return; 
 

 regDepMap.erase(dest);

 instrTmpDepMap.erase(inst_addr);

 if (numOfArgs >=1){

    set_union(regDepMap[dest],arg1);

    if (set_instr){

      if (!KnobLoadTarg){
	set_union(instrDepMap[inst_addr],arg1); 

	set_union(instrTmpDepMap[inst_addr],arg1); 
      }
      else if (ld_set.find(inst_addr) != ld_set.end()){

	set_union(instrDepMap[inst_addr],arg1);

	set_union(instrTmpDepMap[inst_addr],arg1); 
      }
    }
 }

 if (numOfArgs >=2){

    set_union(regDepMap[dest],arg2);

    if (set_instr){

     if (!KnobLoadTarg){
      set_union(instrDepMap[inst_addr],arg2);

      set_union(instrTmpDepMap[inst_addr],arg2); 
     }
     else if (ld_set.find(inst_addr) != ld_set.end()){
      set_union(instrDepMap[inst_addr],arg2); 

      set_union(instrTmpDepMap[inst_addr],arg2); 
     }
    }
 }

 if (numOfArgs >=3){

    set_union(regDepMap[dest],arg3);

    if (set_instr){

      if (!KnobLoadTarg){
	set_union(instrDepMap[inst_addr],arg3); 

	set_union(instrTmpDepMap[inst_addr],arg3); 
      }
      else if (ld_set.find(inst_addr) != ld_set.end()){
	set_union(instrDepMap[inst_addr],arg3); 

	set_union(instrTmpDepMap[inst_addr],arg3); 
      }

    }
 }

 ADDRINT_set tmp;

 if (KnobControlFlow){

   if (!controlDepStack.empty()){

    ADDRINT tmpAddr = controlDepStack.top();

    if (controlDepMap.find(tmpAddr) != controlDepMap.end()){
      set_union(tmp, controlDepMap[tmpAddr]);
      tmp.insert(tmpAddr);
    }

    set_union(regDepMap[dest],tmp);

    if (set_instr){

     if (!KnobLoadTarg){
	set_union(instrDepMap[inst_addr],tmp); 

	set_union(instrTmpDepMap[inst_addr], tmp); 
      }
      else if (ld_set.find(inst_addr) != ld_set.end()){
	set_union(instrDepMap[inst_addr],tmp); 

	set_union(instrTmpDepMap[inst_addr], tmp); 
      }

    }

   }

 }

 



 regDepMap[dest].insert(inst_addr);

}

//setting the dep of memory

void SetDepForMemory(ADDRINT start, ADDRINT size, ADDRINT inst_addr,  bool set_instr,  int numOfArgs, ADDRINT_set &arg1, ADDRINT_set &arg2, ADDRINT_set &arg3)
{
 
 instrTmpDepMap.erase(inst_addr);

 ADDRINT_set tmp;


if (KnobControlFlow){

   if (!controlDepStack.empty()){

    ADDRINT tmpAddr = controlDepStack.top();

    if (controlDepMap.find(tmpAddr) != controlDepMap.end()){
      set_union(tmp, controlDepMap[tmpAddr]);
      tmp.insert(tmpAddr);
    }
   }
 }


  for(ADDRINT addr = start; addr < start + size; addr++) {

     map<ADDRINT, ADDRINT_set >::iterator iter = memDepMap.find(addr);

     memDepMap.erase(addr);

     if (numOfArgs >=1){
       set_union(memDepMap[addr],arg1);  
       if (set_instr){

	 if (!KnobLoadTarg){
	   set_union(instrDepMap[inst_addr],arg1); 

	   set_union(instrTmpDepMap[inst_addr],arg1); 
	 }
	 else if (ld_set.find(inst_addr) != ld_set.end()){
	   set_union(instrDepMap[inst_addr],arg1); 

	   set_union(instrTmpDepMap[inst_addr],arg1); 
	 }

       }
     }

     if (numOfArgs >=2){
	 set_union(memDepMap[addr],arg2); 
	 if (set_instr){

	   if (!KnobLoadTarg){
	     set_union(instrDepMap[inst_addr],arg2); 

	     set_union(instrTmpDepMap[inst_addr],arg2); 
	   }
	   else if (ld_set.find(inst_addr) != ld_set.end()){
	     set_union(instrDepMap[inst_addr],arg2); 

	     set_union(instrTmpDepMap[inst_addr],arg2); 
	   }

	 } 
     }

     if (numOfArgs >=3){
	 set_union(memDepMap[addr],arg3);  
	 if (set_instr){

	   if (!KnobLoadTarg){
	     set_union(instrDepMap[inst_addr],arg3); 

	     set_union(instrTmpDepMap[inst_addr],arg3); 
	   }
	   else if (ld_set.find(inst_addr) != ld_set.end()){
	     set_union(instrDepMap[inst_addr],arg3); 

	     set_union(instrTmpDepMap[inst_addr],arg3); 
	   }

	 }
     }


     if (KnobControlFlow  && !controlDepStack.empty()){
       
       set_union(memDepMap[addr],tmp);

       if (set_instr){

	 if (!KnobLoadTarg){
	   set_union(instrDepMap[inst_addr],tmp);
 
	   set_union(instrTmpDepMap[inst_addr],tmp); 
	 }
	 else if (ld_set.find(inst_addr) != ld_set.end()){
	   set_union(instrDepMap[inst_addr],tmp); 

	   set_union(instrTmpDepMap[inst_addr],tmp); 
	 }
       }

     }

     memDepMap[addr].insert(inst_addr);
    
   }

}

//setting the dep of instr

void SetDepForInstr(ADDRINT inst_addr, int numOfArgs, ADDRINT_set &arg1, ADDRINT_set &arg2, ADDRINT_set &arg3)
{


  instrTmpDepMap.erase(inst_addr);


 if (numOfArgs >=1){

   if (!KnobLoadTarg){
	     set_union(instrDepMap[inst_addr],arg1); 

     	     set_union(instrTmpDepMap[inst_addr],arg1); 

   }
   else if (ld_set.find(inst_addr) != ld_set.end()){
	 set_union(instrDepMap[inst_addr],arg1);

	 set_union(instrTmpDepMap[inst_addr],arg1);  
   }

 }

 if (numOfArgs >=2){

  if (!KnobLoadTarg){
	     set_union(instrDepMap[inst_addr],arg2); 

     	     set_union(instrTmpDepMap[inst_addr],arg2); 
   }
   else if (ld_set.find(inst_addr) != ld_set.end()){
	 set_union(instrDepMap[inst_addr],arg2); 

 	 set_union(instrTmpDepMap[inst_addr],arg2); 
   }
 
 }

 if (numOfArgs >=3){

  if (!KnobLoadTarg){
	     set_union(instrDepMap[inst_addr],arg3); 

     	     set_union(instrTmpDepMap[inst_addr],arg3); 
   }
   else if (ld_set.find(inst_addr) != ld_set.end()){
	 set_union(instrDepMap[inst_addr],arg3); 
    	 set_union(instrTmpDepMap[inst_addr],arg3); 
   }

 }
 


}


//add a branch to control set
void PushControl(ADDRINT addr)
{

  map<REG, ADDRINT_set>::iterator iter = regDepMap.find(LEVEL_BASE::REG_EFLAGS);

  if((regDepMap.end() == iter) || (regDepMap.count(LEVEL_BASE::REG_EFLAGS) == 0) ){
    return;
  }
 

  set_set_entries(controlDepMap[addr],regDepMap[LEVEL_BASE::REG_EFLAGS]); 

}

//pop a branch from a control set
void PopControl(int n, ...)
{

 
  va_list ap;
  ADDRINT addr;

  va_start(ap, n);
  
  for (; n; n--) {
    
    addr = va_arg(ap, ADDRINT);




    if(controlDepMap.end() == controlDepMap.find(addr)) return;
 

 
    controlDepMap.erase(addr);
  }

  va_end(ap);

}


VOID ClearRegDepMap(REG reg){

     if (regDepMap.count(reg) > 0)
       regDepMap.erase(reg);
}

VOID ClearMemDepMap(ADDRINT start, ADDRINT size){
 for(ADDRINT addr = start; addr < start + size; addr++) {
   if (memDepMap.count(addr) > 0)
     memDepMap.erase(addr);
 }
}

VOID ClearSetDepMap(ADDRINT_set &set){

   if (!set.empty())
    set.clear();
}


ADDRINT CheckEqual_r_r(ADDRINT v1, ADDRINT v2)
{
  return v1 == v2;
}

ADDRINT CheckNotEqual_r_r(ADDRINT v1, ADDRINT v2)
{
  return v1 != v2;
}

ADDRINT CheckEqual_m_r(ADDRINT start, ADDRINT size, ADDRINT v2)
{
  ADDRINT v1 = *((ADDRINT *) start);
  return v1 == v2;
}

ADDRINT CheckNotEqual_m_r(ADDRINT start, ADDRINT size, ADDRINT v2)
{
  ADDRINT v1 = *((ADDRINT *) start);
  return v1 == v2;
}


//ADC
//type aware function that calculate
//the dep of instructions and target operands
//we check first the type 
//of the source operands
//and read their dependences
//then we set the dependences of 
//the target operands according
//to the type of the instruction
//we also store the outcome of the instruction
static void Instrument_ADC(INS ins, void* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	   IARG_ADDRINT, INS_OperandReg(ins, 0),
    	   IARG_PTR, &dest_set,
    	   IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {

    if(!INS_IsMemoryRead(ins)) return;    

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);

  }
  else {
    log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
    log.flush();
    abort();
  }
  
 
  if(INS_OperandIsReg(ins, 1)) {
   
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	   IARG_ADDRINT, INS_OperandReg(ins, 1),
    	   IARG_PTR, &src_set,
    	   IARG_END);   

  }
  else if(INS_OperandIsMemory(ins, 1)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END);
   
  }
  else if(INS_OperandIsImmediate(ins, 1)) {
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &src_set,
      		   IARG_END);

  }
  else {
      log << "Unknown operand(1) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }


  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
	 IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
	 IARG_PTR, &eflags,
	 IARG_END);

  //dest <- dest, src, eflags
  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
    		   IARG_ADDRINT, INS_OperandReg(ins, 0),
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
    		   IARG_UINT32, 3,
    		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
		   IARG_PTR, &eflags,
    		   IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(StoreDestRegVali), IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),
    		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 3,
		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
		   IARG_PTR, &eflags,
    		   IARG_END);  

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

    if (INS_HasFallThrough(ins)){
 
      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVali),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }
  
  // Insert call to propagate dependences from dest, src, eflags to eflags
  //eflags <- dest, src, eflags
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
    		   IARG_UINT32, 3,
    		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
		   IARG_PTR, &eflags,
    		   IARG_END);
}


//ADD
static void Instrument_ADD(INS ins, void* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;


 if(INS_OperandIsReg(ins, 0)) {
   INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	   IARG_ADDRINT, INS_OperandReg(ins, 0),
    	   IARG_PTR, &dest_set,
    	   IARG_END);   
 }  
 
 else if(INS_OperandIsMemory(ins, 0)) {
  if(!INS_IsMemoryRead(ins)) return;
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);
 }
 else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }


 if(INS_OperandIsReg(ins, 1)) {
   INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	   IARG_ADDRINT, INS_OperandReg(ins, 1),
    	   IARG_PTR, &src_set,
    	   IARG_END);   
 }  
 
 else if(INS_OperandIsMemory(ins, 1)) {
  if(!INS_IsMemoryRead(ins)) return;
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END);
 }
 else if(INS_OperandIsImmediate(ins,1)) {

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &src_set,
      		   IARG_END);
 }
 else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }

 //dest <- src + dest
 if(INS_OperandIsReg(ins, 0)) {

   INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
    		   IARG_ADDRINT, INS_OperandReg(ins, 0),
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
    		   IARG_UINT32, 2,
    		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
    		   IARG_END);

   INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVali, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);


 }
 else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),
    		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 2,
		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
    		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);


     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVali),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }


 }
 else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }


 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
    		   IARG_UINT32, 2,
    		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
    		   IARG_END);

}

//AND
static void Instrument_AND(INS ins, void* _e)
{
    Instrument_ADD(ins, _e);
}

//CDQ
//CWD/CDQ—Convert Word to Doubleword/Convert Doubleword to Quadword
static void Instrument_CDQ(INS ins, void* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;


  UINT32 operandWidth = INS_OperandWidth(ins, 0);

  if((32 == operandWidth) || (64 == operandWidth)) { 

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
	 IARG_ADDRINT, LEVEL_BASE::REG_EAX,
	 IARG_PTR, &eax,
	 IARG_END);

    // edx <- eax
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
    		   IARG_ADDRINT, LEVEL_BASE::REG_EDX,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
    		   IARG_UINT32, 1,
    		   IARG_PTR, &eax,
    		   IARG_END);
    
    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, LEVEL_BASE::REG_EDX, IARG_PTR, (VOID*)e, IARG_END);

  }
  else {
    log << "Unhandled operand size: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
}

//CLD
//clears the DF flag in the EFLAGS register
static void Instrument_CLD(INS ins, void* _e)
{


  //eflags <- clear
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
    		   IARG_UINT32, 0,
    		   IARG_END);  
}






//CMPSB
//Compares byte at address DS:(E)SI with byte at address
//ES:(E)DI and sets the status flags accordingly
static void Instrument_CMPSB(INS ins, void* _e)
{


  if(!INS_IsMemoryRead(ins)) return;  

  //Memory reference [edi]
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, LEVEL_BASE::REG_EDI,
		   IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		   IARG_PTR, &dest_set,
		   IARG_END);  
  
  //Memory reference [esi]
 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD2_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, LEVEL_BASE::REG_ESI,
		   IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		   IARG_PTR, &src_set,
		   IARG_END);  


  //eflags <- dest, src
 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
    		   IARG_UINT32, 2,
    		   IARG_PTR, &dest_set,
    		   IARG_PTR, &src_set,
    		   IARG_END);  

}

//CMPXCHG: Compare and Exchange
static void Instrument_CMPXCHG(INS ins, void* _e)
{


  UINT32 operandWidth = INS_OperandWidth(ins, 0);

  if((32 == operandWidth) || (64 == operandWidth)) { 

    if(INS_OperandIsReg(ins, 0)) {

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	     IARG_ADDRINT, INS_OperandReg(ins, 0),
	     IARG_PTR, &dest_set,
	     IARG_END);

    }
    else if(INS_OperandIsMemory(ins, 0)) {
      if(!INS_IsMemoryRead(ins)) return;
      
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &dest_set,
		     IARG_END);
    }
    else {
        log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
        log.flush();
        abort();
    }
   

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	     IARG_ADDRINT, INS_OperandReg(ins, 1),
	     IARG_PTR, &src_set,
	     IARG_END);
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 2),
		   IARG_PTR, &eax,
		   IARG_END);
  }
  else {
    log << "Unhanded operand width: " << operandWidth << " for " << INS_Disassemble(ins) << "\n";
    log.flush();
    abort();
  }

   
  if(INS_OperandIsReg(ins, 0)) {
    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckEqual_r_r),
		     IARG_REG_VALUE, INS_OperandReg(ins, 0),
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &src_set,
		       IARG_END);

   INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckEqual_r_r),
		     IARG_REG_VALUE, INS_OperandReg(ins, 0),
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForInstr),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_UINT32, 3,
		       IARG_PTR, &src_set,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &eax,
		       IARG_END);
 

    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckNotEqual_r_r),
		     IARG_REG_VALUE, INS_OperandReg(ins, 0),
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 2),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);


    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckNotEqual_r_r),
		     IARG_REG_VALUE, INS_OperandReg(ins, 0),
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForInstr),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_UINT32, 2,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &eax,
		       IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckEqual_m_r),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);

    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),	  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
		   IARG_UINT32, 1,
    		   IARG_PTR, &src_set,
		   IARG_END);

    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckEqual_m_r),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForInstr),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_UINT32, 3,
		       IARG_PTR, &src_set,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &eax,
		       IARG_END);


    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckNotEqual_m_r),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 2),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);

    INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckNotEqual_m_r),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_REG_VALUE, INS_OperandReg(ins, 2),
		     IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForInstr),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_UINT32, 2,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &eax,
		       IARG_END);


  }
}

//CWDE

static void Instrument_CWDE(INS ins, void* _e)
{



 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		 IARG_ADDRINT, LEVEL_BASE::REG_AX,
		 IARG_PTR, &src_set,
		 IARG_END);
  

  // eax <- ax
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &src_set,
		       IARG_END);

}

//DEC  Decrement by 1
//DEST <- DEST – 1;

static void Instrument_DEC(INS ins, void* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;


  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		 IARG_ADDRINT, INS_OperandReg(ins, 0),
		 IARG_PTR, &dest_set,
		 IARG_END);

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 1,
		   IARG_PTR, &dest_set,
		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }
  
  //eflags <- dest
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);
}


static void Instrument_DIV(INS ins, void* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;


  UINT32 operandWidth = INS_OperandWidth(ins, 0);

  if((32 == operandWidth) || (64 == operandWidth) ) {
    if(INS_OperandIsReg(ins, 0)) {
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);
    }
    else if(INS_OperandIsMemory(ins, 0)) {
      if(!INS_IsMemoryRead(ins)) return;
      
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);
    }
    else {
        log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
        log.flush();
        abort();
    }
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		   IARG_PTR, &eax,
		   IARG_END);
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EDX,
		   IARG_PTR, &edx,
		   IARG_END);
    
    //eax <- eax, edx, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 3,
		       IARG_PTR, &eax,
		       IARG_PTR, &edx,
		       IARG_PTR, &src_set,
		       IARG_END);
    
    //edx <- eax, edx, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EDX,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
		   IARG_UINT32, 3,
		   IARG_PTR, &eax,
		   IARG_PTR, &edx,
		   IARG_PTR, &src_set,
		   IARG_END);

    //eflags <- clear
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearRegDepMap),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		   IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, LEVEL_BASE::REG_EAX, IARG_PTR, (VOID*)e, IARG_END);

  }
  else {
    log << "Unhandled operand size: "<<INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }

}


//IDIV—Signed Divide

static void Instrument_IDIV(INS ins, void* _e)
{


  InsBufferEntry* e = (InsBufferEntry*)_e;

  UINT32 operandWidth = INS_OperandWidth(ins, 0);

  if((32 == operandWidth) ||(64 == operandWidth)) {
    
    if(INS_OperandIsReg(ins, 0)) {

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else if(INS_OperandIsMemory(ins, 0)) {
      if(!INS_IsMemoryRead(ins)) return;

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);
 
    }
    else {
        log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
        log.flush();
        abort();
    }
    
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		   IARG_PTR, &eax,
		   IARG_END);
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EDX,
		   IARG_PTR, &edx,
		   IARG_END);

    // eax <- edx, eax, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 3,
		       IARG_PTR, &eax,
		       IARG_PTR, &edx,
		       IARG_PTR, &src_set,
		       IARG_END);

    // edx <- edx, eax, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EDX,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
		   IARG_UINT32, 3,
		   IARG_PTR, &eax,
		   IARG_PTR, &edx,
		   IARG_PTR, &src_set,
		   IARG_END);

    //eflags <- clear
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearRegDepMap),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		   IARG_END);

    //the value stored in EAX is considered as the outcome of the instr
    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, LEVEL_BASE::REG_EAX, IARG_PTR, (VOID*)e, IARG_END);

  }
  else {
    log << "Unhanded operand width: " << operandWidth << " for " << INS_Disassemble(ins) << "\n";
    log.flush();
    abort();
  }
}

//IMUL—Signed Multiply
static void Instrument_IMUL(INS ins, void* _e)
{

  InsBufferEntry* e = (InsBufferEntry*)_e;

  int operand_count = 0;
  for(unsigned int i = 0; i < INS_OperandCount(ins); i++) {
    if(!INS_OperandIsImplicit(ins, i)) operand_count++;
  }

  if(1 == operand_count) {
    Instrument_MUL(ins, _e);
  }
  else if(2 == operand_count) {
  

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &dest_set,
		     IARG_END);
    
    if(INS_OperandIsReg(ins, 1)) {

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 1),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else if(INS_OperandIsMemory(ins, 1)) {
      if(!INS_IsMemoryRead(ins)) return;

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else if(INS_OperandIsImmediate(ins, 1)) {

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &src_set,
      		   IARG_END);

    }
    else {
        log << "Unknown operand type: " << INS_Disassemble(ins) << "\n";
        log.flush();
      abort();
    }

    //dest <- dest, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &src_set,
		       IARG_PTR, &dest_set,
		       IARG_END);

    //eflags <- dest, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
                   IARG_UINT32, 2,
		   IARG_PTR, &src_set,
		   IARG_PTR, &dest_set,
		   IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(3 == operand_count) {
 

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &dest_set,
		     IARG_END);
    
    if(INS_OperandIsReg(ins, 1)) {

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 1),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else if(INS_OperandIsMemory(ins, 1)) {
      if(!INS_IsMemoryRead(ins)) return;
      
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else {
        log << "Unknown operand type: " << INS_Disassemble(ins) << "\n";
        log.flush();
        abort();
    }

   //dest <- dest, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &src_set,
		       IARG_PTR, &dest_set,
		       IARG_END);

    //eflags <- dest, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
                   IARG_UINT32, 2,
		   IARG_PTR, &src_set,
		   IARG_PTR, &dest_set,
		   IARG_END);  
 
    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);
    
  }
  else {
    printf("\tStrange IMUL %s %d\n", 
	   INS_Disassemble(ins).c_str(), operand_count);
    abort();
  }
}


//LEA—Load Effective Address

static void Instrument_LEA(INS ins, void* _e)
{


  InsBufferEntry* e = (InsBufferEntry*)_e;

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		     IARG_PTR, &base,
		     IARG_END);

 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		     IARG_PTR, &idx,
		     IARG_END);

  // dest <- base, idx
 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &base,
		       IARG_PTR, &idx,
		       IARG_END);

 INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

}



//MOV
static void Instrument_MOV(INS ins, void* _e)
{
 

  InsBufferEntry* e = (InsBufferEntry*)_e;


  //source operand
  if(INS_OperandIsReg(ins, 1)) {



        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	   IARG_ADDRINT, INS_OperandReg(ins, 1),
    	   IARG_PTR, &src_set,
    	   IARG_END);

  }
  else if (INS_OperandIsMemory(ins,1)){
   
    if(!INS_IsMemoryRead(ins)) return;
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END);
  } 
  else if(INS_OperandIsImmediate(ins,1)) {

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &src_set,
      		   IARG_END);



  } 
  else {

      log << "Unknown operand(1) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

   //dest <- src
 
  if(INS_OperandIsReg(ins, 0)) {

    if (!(INS_OperandIsReg(ins, 1) && (INS_OperandReg(ins,1) == INS_OperandReg(ins,0)))){
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
    		   IARG_ADDRINT, INS_OperandReg(ins, 0),
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
    		   IARG_UINT32, 1,
    		   IARG_PTR, &src_set,
    		   IARG_END);

      INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVali, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

    }
  }
  else if (INS_OperandIsMemory(ins,0)){
    if(!INS_IsMemoryWrite(ins)) return;

#ifdef IMPLICIT

        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
    	     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
	     IARG_PTR, &base,
	     IARG_END);

	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &idx,
		   IARG_END);
#endif    

     INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),
    		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
    		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
#ifdef IMPLICIT
           IARG_UINT32, 3,
#else       
           IARG_UINT32, 1,
#endif
    		   IARG_PTR, &src_set,
		   IARG_PTR, &base,
		   IARG_PTR, &idx,
    		   IARG_END);


     INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);


     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVali),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
}

//MOVS/MOVSB/MOVSW/MOVSD—Move Data from String to String

static void Instrument_MOVS(INS ins, void* _e) 
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, LEVEL_BASE::REG_ESI, 
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		     IARG_PTR, &dest_set,
		     IARG_END);
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, LEVEL_BASE::REG_EDI, 
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		     IARG_PTR, &src_set,
		     IARG_END);
    
    if(INS_HasRealRep(ins)) {

	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, LEVEL_BASE::REG_ECX,
		     IARG_PTR, &cnt,
		     IARG_END);

        //dest <- edi, esi, ecx
	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 3,
    		   IARG_PTR, &src_set,
		   IARG_PTR, &dest_set,
		   IARG_PTR, &cnt,
		   IARG_END);


    }else {
    
 	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 2,
    		   IARG_PTR, &src_set,
		   IARG_PTR, &dest_set,
		   IARG_END);
        
    }


    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);


     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

}

static void Instrument_MOVSB(INS ins, void* _e)
{
    Instrument_MOVS(ins, _e);
}

static void Instrument_MOVSD(INS ins, void* _e)
{
  
    Instrument_MOVS(ins, _e);
}

static void Instrument_MOVSW(INS ins, void* _e)
{
  Instrument_MOVS(ins, _e);
}

static void Instrument_MOVSX(INS ins, void* _e)
{
  Instrument_MOV(ins, _e);
}

static void Instrument_MOVZX(INS ins, void* _e)
{
  Instrument_MOV(ins, _e);
}


//MUL

static void Instrument_MUL(INS ins, void* _e)
{




  UINT32 operandWidth = INS_OperandWidth(ins, 0);

  if((32 == operandWidth) || (64 == operandWidth)) {
    if(INS_OperandIsReg(ins, 0)) {
  
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else if(INS_OperandIsMemory(ins, 0)) {
      if(!INS_IsMemoryRead(ins)) return;

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);

    }
    else {
        log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
        log.flush();
        abort();
    }
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		   IARG_PTR, &eax,
		   IARG_END);
    
    
    //eax <- eax, src
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &eax,
		       IARG_PTR, &src_set,
		       IARG_END);

    
    //edx <- eax, src
   INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EDX,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 2,
		       IARG_PTR, &eax,
		       IARG_PTR, &src_set,
		       IARG_END);

    //eflags <- eax, src
   INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		  IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 2,
		       IARG_PTR, &eax,
		       IARG_PTR, &src_set,
		       IARG_END);
  }
  else {

    log << "Unhanded operand width: " << operandWidth << " for " << INS_Disassemble(ins) << "\n";
    log.flush();
    abort();
  }
}

//NEG: Two's Complement Negation

static void Instrument_NEG(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &dest_set,
		     IARG_END);


    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &dest_set,
		     IARG_END);

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 1,
    		   IARG_PTR, &dest_set,
		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);


     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  //eflags <- dest
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS, 
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);

}

static void Instrument_NOP(INS ins, void *v)
{
    //pass -- this instruction doesn't change anything
}

static void Instrument_NOT(INS ins, void *v)
{
  //pass
}

static void Instrument_OR(INS ins, void* _e)
{
  Instrument_ADD(ins, _e);
}

static void Instrument_PAUSE(INS ins, void *v)
{
  //pass
}

//POP: Pop a Value from the Stack

static void Instrument_POP(INS ins, void* _e)
{

  if(!INS_IsMemoryRead(ins)) return;



  InsBufferEntry* e = (InsBufferEntry*)_e;

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		     IARG_PTR, &src_set,
		     IARG_END); 

  //dest <- src
  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &src_set,
		       IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {

    if(!INS_IsMemoryWrite(ins)) return;

#ifdef IMPLICIT

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_PTR, &base,
		   IARG_END);
   
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &idx,
		     IARG_END);

#endif

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
#ifdef IMPLICIT
           IARG_UINT32, 3,
#else
           IARG_UINT32, 1,
#endif
           IARG_PTR, &src_set,
#ifdef IMPLICIT
		   IARG_PTR, &base,
		   IARG_PTR, &idx,
#endif
		   IARG_END);
  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }


  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, LEVEL_BASE::REG_STACK_PTR,
		     IARG_PTR, &esp,
		     IARG_END);

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_STACK_PTR,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &esp,
		       IARG_END);


  INS_InsertCall(ins,
                       IPOINT_BEFORE,
		       AFUNPTR(SrcMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_MEMORYREAD_EA, 
		       IARG_MEMORYREAD_SIZE,
		       IARG_PTR, (VOID*)e,
                       IARG_END);


}

//POPF/POPFD—Pop Stack into EFLAGS Register
static void Instrument_POPFD(INS ins, void* _e)
{
  if(!INS_IsMemoryRead(ins)) return;




  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID, 
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID,
		     IARG_PTR, &src_set,
		     IARG_END);

  //eflags <- top of stack
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &src_set,
		       IARG_END);
}

//PUSH—Push Word or Doubleword Onto the Stack
static void Instrument_PUSH(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		     IARG_PTR, &src_set,
		     IARG_END);

    INS_InsertCall(ins,
                       IPOINT_BEFORE,
		       AFUNPTR(SrcMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_MEMORYREAD_EA, 
		       IARG_MEMORYREAD_SIZE,
		       IARG_PTR, (VOID*)e,
                       IARG_END);

  }
  else if(INS_OperandIsImmediate(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &src_set,
      		   IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreSrcImmVal, IARG_ADDRINT, INS_Address(ins), IARG_UINT32 , INS_OperandIsImmediate(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  } 
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  if(!INS_IsMemoryWrite(ins)) return;
  // dest <- src
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 1,
    		   IARG_PTR, &src_set,
		   IARG_END);

 //we add the following two calls to add the ins to the dependences of ESP
  //TO VERIFY IF WORKING
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, LEVEL_BASE::REG_STACK_PTR,
		     IARG_PTR, &esp,
		     IARG_END);

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_STACK_PTR,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &esp,
		       IARG_END);

}

//PUSHF/PUSHFD—Push EFLAGS Register onto the Stack
static void Instrument_PUSHFD(INS ins, void* _e)
{



  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		     IARG_PTR, &src_set,
		     IARG_END);
  

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 1,
    		   IARG_PTR, &src_set,
		   IARG_END);
}



//RDTSC—Read Time-Stamp Counter
static void Instrument_RDTSC(INS ins, void* _e)
{



  // eax <- clear
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearRegDepMap),
		   IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		   IARG_END);


  // edx <- clear
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearRegDepMap),
		   IARG_ADDRINT, LEVEL_BASE::REG_EDX,
		   IARG_END);

}



//SAL/SAR/SHL/SHR—Shift
static void Instrument_SAR(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END); 

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  if(INS_OperandIsReg(ins, 1)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 1),
		   IARG_PTR, &cnt,
		   IARG_END);
  }
  else if(INS_OperandIsImmediate(ins, 1)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &cnt,
      		   IARG_END);
  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  //dest <- dest, count
  if(INS_OperandIsReg(ins, 0)) {
   
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &cnt,
		       IARG_PTR, &dest_set,
		       IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVali, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory), 
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 2,
		   IARG_PTR, &cnt,
		   IARG_PTR, &dest_set,
		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  //eflags <- dest, count
 INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 2,
		       IARG_PTR, &cnt,
		       IARG_PTR, &dest_set,
		       IARG_END);

}

static void Instrument_SBB(INS ins, void *v)
{
  Instrument_ADC(ins, v);
}

//SCAS/SCASB/SCASW/SCASD—Scan String
static void Instrument_SCASB(INS ins, void* _e)
{



  //Memory reference [edi]
  if(!INS_IsMemoryRead(ins)) return;

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		     IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		     IARG_ADDRINT, LEVEL_BASE::REG_EDI, 
		     IARG_ADDRINT, LEVEL_BASE::REG_INVALID, 
		     IARG_PTR, &src_set,
		     IARG_END);

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, LEVEL_BASE::REG_AL, 
		     IARG_PTR, &eax,
		     IARG_END);
  
  //eflags <- src, al
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS, 
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &src_set,
		       IARG_PTR, &eax,
		       IARG_END);

  if(INS_RepPrefix(ins) || INS_RepnePrefix(ins)) { 

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_ECX, 
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 2,
		       IARG_PTR, &src_set,
		       IARG_PTR, &eax,
		       IARG_END);
  }

}


//Set Byte on Condition

static void Instrument_SETcc(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		     IARG_PTR, &src_set,
		     IARG_END);

  //dest <- src
  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &src_set,
		       IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);
   
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

#ifdef IMPLICIT
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0), 
		     IARG_PTR, &base,
		     IARG_END); 

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		     IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0), 
		     IARG_PTR, &idx,
		     IARG_END);
#endif

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),		  
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
#ifdef IMPLICIT
           IARG_UINT32, 3,
#else
           IARG_UINT32, 1,
#endif
		   IARG_PTR,  &src_set,
#ifdef IMPLICIT
		   IARG_PTR, &idx,
		   IARG_PTR, &base,
#endif		   
		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);


     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
}

static void Instrument_SHL(INS ins, void *v)
{
  Instrument_SAR(ins, v);
}

//SHLD—Double Precision Shift Left

static void Instrument_SHLD(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END); 
    
  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 1),
		   IARG_PTR, &cnt,
		   IARG_END);
  
  if(INS_OperandIsReg(ins, 2)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 2),
		   IARG_PTR, &src_set,
		   IARG_END);

  }
  else if(INS_OperandIsImmediate(ins, 2)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
      		   IARG_ADDRINT, &src_set,
      		   IARG_END);
  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  //dest <- dest, src, count
  if(INS_OperandIsReg(ins, 0)) {
   
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 3,
		       IARG_PTR, &cnt,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &src_set,
		       IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory), 
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 3,
		   IARG_PTR, &cnt,
		   IARG_PTR, &dest_set,
		   IARG_PTR, &src_set,
		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);


     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  //eflags <- dest, src, count
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 3,
		       IARG_PTR, &cnt,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &src_set,
		       IARG_END);
}


static void Instrument_SHR(INS ins, void *v)
{
  Instrument_SAR(ins, v);
}

static void Instrument_SHRD(INS ins, void *v)
{
  Instrument_SHLD(ins, v);
}

//Set Direction Flag

static void Instrument_STD(INS ins, void* _e) 
{



 
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 0,
		       IARG_END);

}

//STMXCSR—Store Streaming SIMD Extension Control/Status

static void Instrument_STMXCSR(INS ins, void* _e)
{



  
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory), 
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
		   IARG_UINT32, 0,
		   IARG_END);
}

//STOS/STOSB/STOSW/STOSD—Store String

static void Instrument_STOSB(INS ins, void* _e) 
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		 IARG_ADDRINT, LEVEL_BASE::REG_AL,
		 IARG_PTR, &eax,
		 IARG_END);

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
	 IARG_ADDRINT, LEVEL_BASE::REG_EDI,
		 IARG_PTR, &src_set,
		 IARG_END);
  
  if(!INS_IsMemoryWrite(ins)) return;

  //dest <- edi, al
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),
		 IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		 IARG_ADDRINT, INS_Address(ins),
		 IARG_BOOL, true,
		 IARG_UINT32, 2,
		 IARG_PTR, &eax,
		 IARG_PTR, &src_set,
		 IARG_END);
 
 //simply we read the value of AL instead of reading memory
  //need to verify that IARG_REG_VALUE can be used with AL
  INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, LEVEL_BASE::REG_AL, IARG_PTR, (VOID*)e, IARG_END);
}

//STOSD Store EAX at address ES:(E)DI
static void Instrument_STOSD(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		 IARG_ADDRINT, LEVEL_BASE::REG_EAX,
		 IARG_PTR, &eax,
		 IARG_END);

  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
	 IARG_ADDRINT, LEVEL_BASE::REG_EDI,
		 IARG_PTR, &src_set,
		 IARG_END);
  
  if(!INS_IsMemoryWrite(ins)) return;

  //dest <- edi, eax
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),
		 IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		 IARG_ADDRINT, INS_Address(ins),
		 IARG_BOOL, true,
		 IARG_UINT32, 2,
		 IARG_PTR, &eax,
		 IARG_PTR, &src_set,
		 IARG_END);

 
  INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, LEVEL_BASE::REG_EAX, IARG_PTR, (VOID*)e, IARG_END);
 
}


static void Instrument_SUB(INS ins, void *v)
{
  Instrument_ADD(ins, v);
}

//TEST—Logical Compare

static void Instrument_TEST(INS ins, void *v)
{



  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		 IARG_ADDRINT, INS_OperandReg(ins, 0),
		 IARG_PTR, &dest_set,
		 IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END); 
  
  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  if(INS_OperandIsReg(ins, 1)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		 IARG_ADDRINT, INS_OperandReg(ins, 1),
		 IARG_PTR, &src_set,
		 IARG_END);

  }
  else if(INS_OperandIsImmediate(ins, 1)) {
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
		   IARG_PTR, &src_set,
		   IARG_END);
  }
  else {
      log << "Unknown operand(1) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  //eflags <- dest, src
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS,
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &src_set,
		       IARG_END);
}

//XADD—Exchange and Add


static void Instrument_XADD(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END); 

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  if(INS_OperandIsReg(ins, 1)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END);
  
  }
  else {
      log << "Unknown operand(1) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
    abort();
  }

  //dest <- dest, src
  if(INS_OperandIsReg(ins, 0)) {
  
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 2,
		       IARG_PTR, &src_set,
		       IARG_PTR, &dest_set,
		       IARG_END);

    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory), 
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 2,
		   IARG_PTR, &src_set,
		   IARG_PTR, &dest_set,
		   IARG_END);

    INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

     if (INS_HasFallThrough(ins)){

      INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  //src <- dest
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 1),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);


  //eflags <- dest, src
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS, 
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, false,
		       IARG_UINT32, 2,
		       IARG_PTR, &dest_set,
		       IARG_PTR, &src_set,
		       IARG_END);
}

//XCHG—Exchange Register/Memory with Register

static void Instrument_XCHG(INS ins, void *v)
{



  if(INS_OperandIsReg(ins, 0)) {
 
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END); 
  
  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  if(INS_OperandIsReg(ins, 1)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END); 

  }
  else if(INS_OperandIsMemory(ins, 1)) {
    if(!INS_IsMemoryRead(ins)) return;
   
     INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END); 

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }

  //dest <- src
  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 0),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &src_set,
		       IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryWrite(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory), 
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 1,
		   IARG_PTR, &src_set,
		   IARG_END);

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  //src <- dest
  if(INS_OperandIsReg(ins, 1)) {
  
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		       IARG_ADDRINT, INS_OperandReg(ins, 1),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_BOOL, true,
		       IARG_UINT32, 1,
		       IARG_PTR, &dest_set,
		       IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 1)) {
    if(!INS_IsMemoryWrite(ins)) return;
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory), 
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 1,
		   IARG_PTR, &dest_set,
		   IARG_END);

  }
  else {
      log << "Unknown operand(1) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
}

//XOR—Logical Exclusive OR

static void Instrument_XOR(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

  if(INS_OperandIsReg(ins, 0)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END);
  }
  else if(INS_OperandIsMemory(ins, 0)) {
    if(!INS_IsMemoryRead(ins)) return;
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 0),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 0),
		   IARG_PTR, &dest_set,
		   IARG_END); 

  }
  else {
      log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  if(INS_OperandIsReg(ins, 1)) {

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END);

  }
  else if(INS_OperandIsMemory(ins, 1)) {
    if(!INS_IsMemoryRead(ins)) return;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DepForMemory),
		   IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE,
		   IARG_ADDRINT, INS_OperandMemoryBaseReg(ins, 1),
		   IARG_ADDRINT, INS_OperandMemoryIndexReg(ins, 1),
		   IARG_PTR, &src_set,
		   IARG_END); 
   
  }
  else if(INS_OperandIsImmediate(ins, 1)) {
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearSetDepMap),
		   IARG_PTR, &src_set,
		   IARG_END);
  }
  else {
      log << "Unknown operand(1) type: " << INS_Disassemble(ins) << "\n";
      log.flush();
      abort();
  }
  
  if(INS_OperandIsReg(ins, 0) && INS_OperandIsReg(ins, 1)
     && INS_OperandReg(ins, 0) == INS_OperandReg(ins, 1)) {
    
    //dest <- clear
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearRegDepMap),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_END);
  }
  else {
    //dest <- dest, src
    if(INS_OperandIsReg(ins, 0)) {
 
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 2,
		   IARG_PTR, &dest_set,
		   IARG_PTR, &src_set,
		   IARG_END);

      INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

    }
    else if(INS_OperandIsMemory(ins, 0)) {
      if(!INS_IsMemoryWrite(ins)) return;
    
      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForMemory),
		   IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, true,
		   IARG_UINT32, 2,
    		   IARG_PTR, &src_set,
    		   IARG_PTR, &dest_set,
		   IARG_END);

      INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

     if (INS_HasFallThrough(ins)){

       INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

    }
    else {
        log << "Unknown operand(0) type: " << INS_Disassemble(ins) << "\n";
        log.flush();
        abort();
    }
  }
  
  //eflags <- dest, src
  INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetDepForRegister),
		   IARG_ADDRINT, LEVEL_BASE::REG_EFLAGS, 
		   IARG_ADDRINT, INS_Address(ins),
		   IARG_BOOL, false,
		   IARG_UINT32, 2,
		   IARG_PTR, &dest_set,
		   IARG_PTR, &src_set,
		   IARG_END);

}

//FSTSW/FNSTSW—Store Status Word

static void Instrument_FNSTSW(INS ins, void* _e)
{



  InsBufferEntry* e = (InsBufferEntry*)_e;

    if (INS_OperandIsReg(ins, 0)) {
       
	INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearRegDepMap),
		   IARG_ADDRINT, INS_OperandReg(ins, 0),
		   IARG_END);

	INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)StoreDestRegVal, IARG_ADDRINT, INS_Address(ins), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_PTR, (VOID*)e, IARG_END);

    }
    else if (INS_OperandIsMemory(ins, 0)) {
      if(!INS_IsMemoryWrite(ins)) return;

      INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ClearMemDepMap),
		     IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE,
		     IARG_END);

      INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

     if (INS_HasFallThrough(ins)){

       INS_InsertCall(ins,
                       IPOINT_AFTER,
		       AFUNPTR(DestMemVal),
		       IARG_ADDRINT, INS_Address(ins),
		       IARG_PTR, (VOID*)e,
                       IARG_END);
     }

    }
}

//get the dep of a branch
//insert it in the conrtrol dep set
//push it in the stack
VOID CONDBr(ADDRINT inst_addr, VOID* _e) {

  map<REG, ADDRINT_set>::iterator iter = regDepMap.find(LEVEL_BASE::REG_EFLAGS);

  if(!((regDepMap.end() == iter) || (regDepMap.count(LEVEL_BASE::REG_EFLAGS) == 0)) ){
    set_set_entries(controlDepMap[inst_addr],regDepMap[LEVEL_BASE::REG_EFLAGS]);    
  }
 
  if (!controlDepStack.empty()){

    ADDRINT tmp = controlDepStack.top();

    if ( tmp != inst_addr){
      if ( (BrIpdomMap.find(tmp) != BrIpdomMap.end()) && (BrIpdomMap.find(inst_addr) != BrIpdomMap.end())){
	if (BrIpdomMap[tmp] ==  BrIpdomMap[inst_addr]){
	  controlDepStack.pop();
	  controlDepMap.erase(tmp);
	  controlDepStack.push(inst_addr);

	}
	else {
	  controlDepStack.push(inst_addr);
	}
  
      }
    }
  }
  else {
	controlDepStack.push(inst_addr);
  }
 
}
  
//pop a branch from the stack
//when its ipdom is encountered
VOID POPCONDBr(ADDRINT inst_addr, VOID* _e) {


  if (!controlDepStack.empty()){
    ADDRINT tmp = controlDepStack.top();

      if ( BrIpdomMap.find(tmp) != BrIpdomMap.end()){
	if (BrIpdomMap[tmp] ==  inst_addr){
	  controlDepStack.pop();
	  controlDepMap.erase(tmp);
	}
    }
  }
}
  



void UnimplementedInstruction(INS ins, VOID* _e)
{
  log<<"unimplemented: " <<INS_Disassemble(ins) << "\n";
  log.flush();
  
  return;
}



