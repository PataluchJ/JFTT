# Debug do programów napisanych na maszynę wirtualną
# JFTT PWR 2021/2022
# Autor: Jakub Patałuch
# requirements:
#   asciimatics
# Program przyjmuje plik jako pierwszy argument 
# np. python debuger.py code.asm
# Sterowanie: 
# n - następna instrukcja
# b - cofnij instrukcję
# w - rusz wskaźnik wyboru instrukcji w górę
# s - rusz wskaźnik wybory instrukcji w dół
# d - dodaj breakpoint na wybranej lini 
# c - wykonuj kod do napotkania breakpointu 
# r - reset maszyny wirtualnej BREAKPOINTY ZOSTAJĄ 
# q - wyłącza debugere 
# Uwagi: Stos poprzednich wartości rejestrów/pamięci/instrukcji 
#  rośnie bez przerwy, w przypadku dużej ilości wykonań pętli 
#  może zabraknąć RAMu. Wyłączyć można to zmieniając poniższą zmienną 
CREATE_BACKSTACK = True 
# Lista wypisanych wartości przez VM nie jest cofana w przypadku 
#  cofnięcia instrukcji! 
# Program automatycznie dodaje breakpointy na HALTach, można to wyłączyć niżej 
AUTO_HALT_BREAKPOINT = True

from math import floor
import sys
import copy
from asciimatics.screen import Screen
from asciimatics.screen import ManagedScreen

class Debuger:
    def __init__(self):
        self.COLOUR_BLACK = 0
        self.COLOUR_RED = 1
        self.COLOUR_GREEN = 2
        self.COLOUR_YELLOW = 3
        self.COLOUR_BLUE = 4
        self.COLOUR_MAGENTA = 5
        self.COLOUR_CYAN = 6
        self.COLOUR_WHITE = 7

        self.A_BOLD = 1
        self.A_NORMAL = 2
        self.A_REVERSE = 3
        self.A_UNDERLINE = 4

        self.registersContent = [90909090909 for i in range(8)]
        self.currentInstruction = 0
        self.printedByMachine = []
        self.halted = False
        self.memory = {0:8080808080}
        self.memory.pop(0)
        self.autoStep = False 
        self.focusLocket = True

        self.prevRegisters = []
        self.prevInstruction = []
        self.prevMemory = []
        self.backStackSize = 0

        self.registerNames = ["a", "b", "c","d","e","f","g","h"]
        self.screen = ManagedScreen()

    def stringToReg(self, str):
        for i in range(len(self.registerNames)):
            if self.registerNames[i] == str:
                return i
        

    def applyInstruction(self, command):
        try:
            if CREATE_BACKSTACK:
                self.prevInstruction.append(copy.deepcopy(self.currentInstruction))
                self.prevMemory.append(copy.deepcopy(self.memory))
                self.prevRegisters.append(copy.deepcopy(self.registersContent))
                self.backStackSize += 1
            inst = command.split()
            if inst[0] == "GET":
                self.screen.print_at("Input: ", 40, 0, bg=self.COLOUR_RED)
                self.screen.refresh()
                self.screen.clear_buffer(fg=self.COLOUR_WHITE, bg=self.COLOUR_BLACK, attr=2)
                val = input("Input: ")
                self.registersContent[0] = int(val)
                self.currentInstruction += 1
                
            elif inst[0] == "PUT":
                self.printedByMachine.append(self.registersContent[0])
                self.currentInstruction += 1
                
            elif inst[0] == "RESET":
                reg = self.stringToReg(inst[1])
                self.registersContent[reg] = 0
                self.currentInstruction += 1
                
            elif inst[0] == "LOAD":
                reg = self.stringToReg(inst[1])
                if self.registersContent[reg] not in self.memory:
                    self.registersContent[0] = 8080808080
                self.registersContent[0] = self.memory[self.registersContent[reg]]
                self.currentInstruction += 1

            elif inst[0] == "STORE":
                reg = self.stringToReg(inst[1])
                self.memory[self.registersContent[reg]] = self.registersContent[0]
                self.currentInstruction += 1

            elif inst[0] == "ADD":
                reg = self.stringToReg(inst[1])
                self.registersContent[0] += self.registersContent[reg]
                self.currentInstruction += 1
                
            elif inst[0] == "SUB":
                reg = self.stringToReg(inst[1])
                self.registersContent[0] -= self.registersContent[reg]
                self.currentInstruction += 1
                
            elif inst[0] == "SHIFT":
                reg = self.stringToReg(inst[1])
                self.registersContent[0] = floor(self.registersContent[0] * 2**self.registersContent[reg])
                self.currentInstruction += 1
                
            elif inst[0] == "SWAP":
                reg = self.stringToReg(inst[1])
                t = self.registersContent[reg]
                self.registersContent[reg] = self.registersContent[0]
                self.registersContent[0] = t
                self.currentInstruction += 1
                
            elif inst[0] == "RESET":
                reg = self.stringToReg(inst[1])
                self.registersContent[reg] = 0
                self.currentInstruction += 1
                
            elif inst[0] == "INC":
                reg = self.stringToReg(inst[1])
                self.registersContent[reg] += 1
                self.currentInstruction += 1
                
            elif inst[0] == "DEC":
                reg = self.stringToReg(inst[1])
                self.registersContent[reg] -= 1
                self.currentInstruction += 1
                
            elif inst[0] == "JUMP":
                j = int(inst[1])
                self.currentInstruction += j
                
            elif inst[0] == "JPOS":
                if self.registersContent[0] > 0:
                    j = int(inst[1])
                    self.currentInstruction += j
                else:
                    self.currentInstruction += 1
                
            elif inst[0] == "JZERO":
                if self.registersContent[0] == 0:
                    j = int(inst[1])
                    self.currentInstruction += j
                else:
                    self.currentInstruction += 1

            elif inst[0] == "JNEG":
                if self.registersContent[0] < 0:
                    j = int(inst[1])
                    self.currentInstruction += j
                else:
                    self.currentInstruction += 1

            elif inst[0] == "HALT":
                self.halted = True

            else:
                print(inst[0] + " unknow instruction.")

            return True 
        except Exception as inst: 
            self.printedByMachine.append("Error at line " + str(self.currentInstruction))

            return False 
    def reset(self):
        self.registersContent = [90909090909 for i in range(8)]
        self.currentInstruction = 0
        self.printedByMachine = []
        self.halted = False
        self.memory = {0:8080808080}
        self.memory.pop(0)
        self.autoStep = False 
        self.prevInstruction = []
        self.prevMemory = [] 
        self.prevRegisters = []
    def userInput(self):
        pass
    def ui(self, screen):
        self.screen = screen
        if(len(sys.argv) < 2):
            print("Input file not specyfied!")
            return
        f = open(sys.argv[1])
        instructions = f.read().split('\n')

        maxOnScreen = screen.height
        dataColumnStart = 17
        instructionListPosition = 0
        breakpointAt = [False for i in range(len(instructions))]
        if AUTO_HALT_BREAKPOINT:
            for i in range(len(instructions)):
                if instructions[i] == "HALT":
                    breakpointAt[i] = True

        currentColor = self.COLOUR_GREEN
        while not self.halted:
            if self.currentInstruction == len(instructions):
                return
            if self.focusLocket: 
                instructionListPosition = self.currentInstruction
            instructionsUpperBound = int(max(0, instructionListPosition - maxOnScreen/2))
            instructionsLowerBound = min(instructionsUpperBound + maxOnScreen, len(instructions))
            index = instructionsUpperBound
            line = 0
            brInfo = '#'
            # Print instruction list
            while index < instructionsLowerBound:
                brInfo = ' '
                if breakpointAt[index]:
                    brInfo = 'B'
                
                if index == self.currentInstruction:
                    screen.print_at(brInfo + str(index).rjust(4) + ">" + str(instructions[index]).ljust(dataColumnStart-6), 0, line, bg=currentColor)
                elif index == instructionListPosition:
                    screen.print_at(brInfo + str(index).rjust(4) + " " + str(instructions[index]).ljust(dataColumnStart-6), 0, line, bg=self.COLOUR_CYAN)
                else:
                    screen.print_at(brInfo + str(index).rjust(4) + " " + str(instructions[index]).ljust(dataColumnStart-6), 0, line)
                screen.print_at("|", dataColumnStart-1, line)
                
                line += 1
                index += 1
            # Print registers value 
            
            screen.print_at(" Registers content: ", dataColumnStart, 0)
            line = 1
            for i in range(len(self.registersContent)):
                screen.print_at(" " + self.registerNames[i] + ": " + str(self.registersContent[i]).ljust(15), dataColumnStart, line)
                line += 1
            # Print user memory
            screen.print_at(" Memory content: ", dataColumnStart, line)
            line += 1
            for k,v in sorted(self.memory.items()):
                screen.print_at(" " + str(k) + ": " + str(v), dataColumnStart, line)
                line += 1
            ev = screen.get_key()

            # Print output
            line = 2
            screen.print_at("VM Output:", 40, line-1)
            for i in (range(len(self.printedByMachine))):
                screen.print_at(self.printedByMachine[i], 40, line)
                line += 1
            if self.autoStep:
                if breakpointAt[self.currentInstruction]:
                    self.autoStep = False
                else: 
                    if self.applyInstruction(instructions[self.currentInstruction]):
                        if instructionListPosition == self.currentInstruction:
                            instructionListPosition += 1
                    else:
                        currentColor = self.COLOUR_RED
                        self.autoStep = False

            if ev in (ord('n'), ord('N')):
                currentColor = self.COLOUR_GREEN
                self.applyInstruction(instructions[self.currentInstruction])
                if instructionListPosition == self.currentInstruction:
                    instructionListPosition += 1
            if ev in (ord('b'), ord('B')):
                if self.backStackSize > 0:
                    self.registersContent = self.prevRegisters.pop()
                    prevInstVal = self.prevInstruction.pop()
                    if instructionListPosition == self.currentInstruction:
                        instructionListPosition = prevInstVal
                    self.currentInstruction = prevInstVal
                    self.memory = self.prevMemory.pop()
                    self.backStackSize -= 1
            if ev in (ord('w'), ord('W')):
                instructionListPosition = max(0, instructionListPosition-1)
            if ev in (ord('s'), ord('S')):
                instructionListPosition = min(len(instructions), instructionListPosition+1)
            if ev in (ord('d'), ord('D')):
                breakpointAt[instructionListPosition] = not breakpointAt[instructionListPosition]
            if ev in (ord('r'), ord('R')):
                self.reset()
                currentColor = self.COLOUR_GREEN
                instructionListPosition = 0
            if ev in (ord('c'), ord('C')):
                self.autoStep = True 
            if ev in (ord('q'), ord('Q')):
                self.halted = True 
            if ev in (ord('z'), ord('Z')):
                if self.focusLocket:
                    self.focusLocket = False
                else:
                    self.focusLocket = True 
            screen.refresh()
            screen.clear_buffer(fg=self.COLOUR_WHITE, bg=self.COLOUR_BLACK, attr=2, w=screen.width, h=screen.height)

        for o in self.printedByMachine:
            print(o)    
        
deb = Debuger()

Screen.wrapper(deb.ui)

