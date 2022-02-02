from atexit import register
import math
import copy

UNUSED_REGISTER_VALUE = 90909090
CREATE_BACK_STACK = True 
BACKSTEP_STACK_MAX_SIZE = 100

class VirtualMachine:
    def __init__(self):
        # Temporary variables
        urv = UNUSED_REGISTER_VALUE
        # ~Hardware
        self.registerCount = 8
        self.registers = {'a': urv, 'b':urv, 'c':urv, 'd':urv,'e':urv,'f':urv,'g':urv,'h':urv}
        self.memory = dict()
        self.instructionCounter = 0
        self.hasHalted = False 
        self.waitingForInput = False
        self.printed = []
        # Instructions
        self.instructions = {
            "GET": self.inst_GET, "PUT": self.inst_PUT, "LOAD": self.inst_LOAD, "STORE": self.inst_STORE, 
            "ADD": self.inst_ADD, "SUB": self.inst_SUB, "SHIFT":self.inst_SHIFT, "SWAP": self.inst_SWAP, 
            "RESET": self.inst_RESET, "INC": self.inst_INC, "DEC": self.inst_DEC, "JUMP": self.inst_JUMP, 
            "JPOS": self.inst_JPOS, "JZERO":self.inst_JZERO, "JNEG":self.inst_JNEG, "HALT": self.inst_HALT
            }
        self.properties = {
            "GET": {"Arg": None, "Cost": 100}, "PUT": {"Arg": None, "Cost": 100}, 
            "LOAD": {"Arg": str, "Cost": 50}, "STORE": {"Arg": str, "Cost": 50}, 
            "ADD": {"Arg": str, "Cost": 10}, "SUB": {"Arg": str, "Cost": 10}, 
            "SHIFT": {"Arg": str, "Cost": 5}, "SWAP": {"Arg": str, "Cost": 1}, 
            "RESET": {"Arg": str, "Cost": 1}, "INC": {"Arg": str, "Cost": 1}, 
            "DEC": {"Arg": str, "Cost": 1}, "JUMP": {"Arg": int, "Cost": 1}, 
            "JPOS": {"Arg": int, "Cost": 1}, "JZERO": {"Arg": int, "Cost": 1}, 
            "JNEG": {"Arg": int, "Cost": 1}, "HALT": {"Arg": None, "Cost": 0}
        }
        # Exec data
        self.program = []
        # Statistics
        self.instrucionUsage = {
            "GET": 0, "PUT": 0, "LOAD": 0, "STORE": 0, 
            "ADD": 0, "SUB": 0, "SHIFT":0, "SWAP": 0, 
            "RESET": 0, "INC": 0, "DEC": 0, "JUMP": 0, 
            "JPOS": 0, "JZERO": 0, "JNEG": 0, "HALT": 0
            }
        # Run status 
        self.error = False
        # Backstep 
        self.backlog = []

    def input(self,v):
        self.waitingForInput = False
        self.registers['a'] = v
        self.instructionCounter += 1

    def loadProgram(self, instructionList):
        for inst in instructionList:
            if inst != "":
                self.program.append(inst)

    def isValid(self, inst, arg) -> bool:
        if inst not in self.instructions:
            self.printed("Error at line " + str(self.instructionCounter) +
                    ": " + inst + " is not a valid instruction.")
            return False
        if arg == None:
            return self.properties[inst]["Arg"] == None
        if self.properties[inst]["Arg"] == str:
            return arg in self.registers
        elif self.properties[inst]["Arg"] == int:
            try:
                v = int(arg)
                return True
            except:
                return False 
        return False 

    def step(self) -> bool:
        if self.error == True:
            self.error = False
            self.instructionCounter += 1
        if self.hasHalted == True:
            return False 
        spl = self.program[self.instructionCounter].split()
        inst = spl[0] 
        arg = None
        if len(spl) > 1: 
            arg = spl[1]
        if not self.isValid(inst,arg):
            self.error = True 
            return False 
        
        if CREATE_BACK_STACK == True:
            if len(self.backlog) >= BACKSTEP_STACK_MAX_SIZE:
                del self.backlog[0]
            log = {'REG': copy.deepcopy(self.registers), 'MEM': copy.deepcopy(self.memory), 
                    'INST': copy.deepcopy(self.instructionCounter)}
            self.backlog.append(log)

        if self.properties[inst]['Arg'] == None:
            return self.instructions[inst]()
        elif self.properties[inst]['Arg'] == str:
            return self.instructions[inst](arg)
        else:
            v = int(arg)
            return self.instructions[inst](v)
        

    def backstep(self):
        if CREATE_BACK_STACK == False:
            return
        if len(self.backlog) == 0:
            return 
        self.hasHalted = False 
        top = self.backlog[len(self.backlog)-1]
        self.memory = top['MEM']
        self.registers = top['REG']
        self.instructionCounter = top['INST']
        self.backlog.pop()

    def inst_GET(self) -> bool:
        #self.registers['a'] = v
        #self.instructionCounter += 1
        self.waitingForInput = True
        self.instrucionUsage['GET'] += 1
        return True

    def inst_PUT(self):
        self.printed.append(str(self.registers['a']))
        self.instructionCounter += 1
        self.instrucionUsage['PUT'] += 1
        return True

    def inst_LOAD(self,x):
        address = self.registers[x]
        self.instrucionUsage['LOAD'] += 1
        if address in self.memory:
            self.registers['a'] = self.memory[address]
            self.instructionCounter += 1
            return True 
        else:
            self.registers['a'] = 0
            self.printed.append("Read not init mem.")
            return False
         
    def inst_STORE(self,x):
        self.memory[self.registers[x]] = self.registers['a']
        self.instructionCounter += 1
        self.instrucionUsage['STORE'] += 1
        return True

    def inst_ADD(self,x):
        self.registers['a'] += self.registers[x]
        self.instructionCounter += 1
        self.instrucionUsage['ADD'] += 1
        return True

    def inst_SUB(self,x):
        self.registers['a'] -= self.registers[x]
        self.instructionCounter += 1
        self.instrucionUsage['SUB'] += 1
        return True
        
    def inst_SHIFT(self,x):
        self.registers['a'] = math.floor(self.registers['a']*(2**(self.registers[x])))
        self.instructionCounter += 1
        self.instrucionUsage['SHIFT'] += 1
        return True
        
    def inst_SWAP(self,x):
        t = self.registers['a']
        self.registers['a'] = self.registers[x]
        self.registers[x] = t
        self.instructionCounter += 1
        self.instrucionUsage['SWAP'] += 1
        return True
        
    def inst_RESET(self,x):
        self.registers[x] = 0
        self.instructionCounter += 1
        self.instrucionUsage['RESET'] += 1 
        return True

    def inst_INC(self,x):
        self.registers[x] += 1
        self.instructionCounter += 1
        self.instrucionUsage['INC'] += 1
        return True

    def inst_DEC(self,x):
        self.registers[x] -= 1 
        self.instructionCounter += 1
        self.instrucionUsage['DEC'] += 1
        return True

    def inst_JUMP(self,j):
        self.instructionCounter += j
        self.instrucionUsage['JUMP'] += 1
        return True
        
    def inst_JPOS(self,j):
        if self.registers['a'] > 0:
            self.instructionCounter += j
        else:
            self.instructionCounter += 1
        self.instrucionUsage['JPOS'] += 1
        return True

    def inst_JZERO(self,j):
        if self.registers['a'] == 0:
            self.instructionCounter += j
        else:
            self.instructionCounter += 1 
        self.instrucionUsage['JZERO'] += 1
        return True

    def inst_JNEG(self,j):
        if self.registers['a'] < 0:
            self.instructionCounter += j
        else:
            self.instructionCounter += 1
        self.instrucionUsage['JNEG'] += 1
        return True

    def inst_HALT(self):
        self.hasHalted = True
        return True
        