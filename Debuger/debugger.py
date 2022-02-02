# Debuger 2.0 
import VM
import sys
from asciimatics.screen import Screen
from asciimatics.screen import ManagedScreen

COLOR_BLACK = 0
COLOR_RED = 1
COLOR_GREEN = 2
COLOR_YELLOW = 3
COLOR_BLUE = 4
COLOR_MAGENTA = 5
COLOR_CYAN = 6
COLOR_WHITE = 7

A_BOLD = 1
A_NORMAL = 2
A_REVERSE = 3
A_UNDERLINE = 4

class Debuger:
    def __init__(self, inst, screen):
        self.machine = VM.VirtualMachine()
        self.machine.loadProgram(inst)
        self.screen = screen
        self.inputAccepted = False 
        self.inputNeeded = False
        self.input = ""
        # Control
        self.quit = False 
        self.autoStep = False
        self.focusLocked = True
        self.cursorPos = 0
        self.breakpoints = set()
        
        # TUI style
        # Colors
        self.TUI_BACKGROUND = COLOR_BLACK
        self.TUI_NORMAL = COLOR_WHITE
        self.TUI_ERROR = COLOR_RED
        self.TUI_INSTR = COLOR_GREEN
        self.TUI_CURSOR = COLOR_CYAN
        self.TUI_BORDER = COLOR_WHITE
        self.TUI_LABEL = COLOR_BLACK
        # TUI 
        self.resizeTUI()

    def handleEvent(self,ev):
        if self.inputNeeded:
            if ev == 13:
                self.inputAccepted = True
            if ( ev == -300 or ev == -1 ) and len(self.input)>0:
                self.input = self.input[:-1]
            
            if ev is not None and ev >= 32:
                self.input = self.input + chr(ev)
        else:
            if ev in (ord('n'), ord('N')):
                self.machine.step()
            if ev in (ord('b'), ord('B')):
                self.machine.backstep() 
            if ev in (ord('z'), ord('Z')):
                self.focusLocked = not self.focusLocked
                self.cursorPos = self.machine.instructionCounter
            if ev in (ord('w'), ord('W')):
                self.focusLocked = False
                self.cursorPos -= 1
                if self.cursorPos < 0:
                    self.cursorPos = len(self.machine.program)-1
            if ev in (ord('s'), ord('s')):
                self.focusLocked = False
                self.cursorPos += 1
                if self.cursorPos > len(self.machine.program)-1:
                    self.cursorPos = 0 
            if ev in (ord('c'), ord('C')):
                self.autoStep = not self.autoStep
            if ev in (ord('d'), ord('D')):
                if self.cursorPos in self.breakpoints:
                    self.breakpoints.remove(self.cursorPos)
                else:
                    self.breakpoints.add(self.cursorPos)
            if ev in (ord('q'), ord('Q')):
                self.quit = True 
            if ev in (ord('r'), ord('R')):
                pass 

    def resizeTUI(self):
        self.TUI_SCREEN_WIDTH = self.screen.dimensions[1]
        self.TUI_SCREEN_HEIGHT = self.screen.dimensions[0]
        
        self.TUI_COLUMN_HEIGHT = self.TUI_SCREEN_HEIGHT - 2
        self.TUI_INST_LIST_WIDTH = 18
        self.TUI_PRINT_WIDTH = 20
        self.TUI_STATISTIC_WIDTH = 16
        self.TUI_VAR_WIDTH = self.TUI_SCREEN_WIDTH - self.TUI_INST_LIST_WIDTH - self.TUI_PRINT_WIDTH - self.TUI_STATISTIC_WIDTH
        
        self.TUI_INST_LIST_START = 0
        self.TUI_PRINT_START = self.TUI_SCREEN_WIDTH - self.TUI_PRINT_WIDTH - 1
        self.TUI_STATISTIC_START = self.TUI_PRINT_START - self.TUI_STATISTIC_WIDTH
        self.TUI_VAR_START = self.TUI_INST_LIST_START+self.TUI_INST_LIST_WIDTH
        self.TUI_VAR_WIDTH = self.TUI_STATISTIC_START - self.TUI_VAR_START
        self.TUI_VAR_COL_WIDTH = int(self.TUI_VAR_WIDTH / 2)

    def drawBox(self,x1,y1,x2,y2,label):
        self.screen.move(x1,y1)
        self.screen.draw(x1,y2, char=' ', bg=self.TUI_BORDER)
        #self.screen.move(x1,y2)
        self.screen.draw(x2,y2, char=' ', bg=self.TUI_BORDER)
        #self.screen.move(x2,y2)
        self.screen.draw(x2,y1, char=' ', bg=self.TUI_BORDER)
        #self.screen.move(x2,y1)
        self.screen.draw(x1,y1, char=' ', bg=self.TUI_BORDER)
        desiredLength = (x2-x1)-4
        if len(label) > desiredLength:
            label = label[0:desiredLength]
        #else:
        #    label = label.center(desiredLength,' ')
        self.screen.print_at(label,(x1+int((x2-x1)/2))-int(len(label)/2), y1, bg=self.TUI_BORDER, colour=self.TUI_LABEL)

    def drawInstructionList(self):
        lowerBound = max(0, int(self.cursorPos - (self.TUI_COLUMN_HEIGHT/2)))
        upperBound = min(lowerBound + self.TUI_COLUMN_HEIGHT, len(self.machine.program))
        self.drawBox(0,0,self.TUI_INST_LIST_WIDTH,self.TUI_SCREEN_HEIGHT-1, 'Instructions')
        line = 1
        for i in range(lowerBound,upperBound):
            breakpointSymbol = ' '
            if i in self.breakpoints:
                breakpointSymbol = 'B'
            if i == self.machine.instructionCounter:
                color = self.TUI_INSTR
                if self.machine.error:
                    color = self.TUI_ERROR
                self.screen.print_at(breakpointSymbol + str(i).rjust(4) + ">" + 
                str(self.machine.program[i]).ljust(self.TUI_INST_LIST_WIDTH-6-1), 1, line, bg= color)
            elif i == self.cursorPos:
                self.screen.print_at(breakpointSymbol + str(i).rjust(4) + " " + 
                str(self.machine.program[i]).ljust(self.TUI_INST_LIST_WIDTH-6-1),1, line, bg= self.TUI_CURSOR)
            else:
                self.screen.print_at(breakpointSymbol + str(i).rjust(4) + " " + 
                str(self.machine.program[i]).ljust(self.TUI_INST_LIST_WIDTH-6-1), 1, line, bg= self.TUI_BACKGROUND)
            line += 1 
    
    def drawPrintLog(self):
        self.drawBox(self.TUI_PRINT_START, 0, self.TUI_PRINT_START+self.TUI_PRINT_WIDTH, self.TUI_SCREEN_HEIGHT-1, 'Printed')
        indx = len(self.machine.printed) - 1
        line = 1
        while indx >= 0 and line < self.TUI_COLUMN_HEIGHT-1:
            self.screen.print_at(">"+self.machine.printed[indx][0:self.TUI_PRINT_WIDTH-1],self.TUI_PRINT_START+1, line, bg= self.TUI_BACKGROUND)
            indx -= 1
            line += 1

    def drawStatistics(self):
        self.drawBox(self.TUI_STATISTIC_START, 0, self.TUI_STATISTIC_START+self.TUI_STATISTIC_WIDTH, self.TUI_SCREEN_HEIGHT-1, 'Statistic')
        st = int(self.TUI_STATISTIC_START + 2)
        line = 1
        for key, val in self.machine.instrucionUsage.items():
            rec = key.rjust(5) + ":" + str(val*self.machine.properties[key]['Cost'])
            rec = rec.ljust(self.TUI_STATISTIC_WIDTH-2)
            self.screen.print_at(rec, st , line, bg= self.TUI_BACKGROUND)
            line += 1
            
    def drawRegAndMem(self):
        self.drawBox(self.TUI_VAR_START, 0, self.TUI_VAR_START+self.TUI_VAR_WIDTH, self.TUI_SCREEN_HEIGHT-1, 'Memory')
        line = 1
        st = self.TUI_VAR_START+2
        # Registers 
        for k,v in self.machine.registers.items():
            pline = k + ": " + str(v)
            self.screen.print_at(pline, st, line, bg= self.TUI_BACKGROUND)
            line += 1
            if line > self.TUI_COLUMN_HEIGHT-1:
                line = 1
                st = st + self.TUI_VAR_COL_WIDTH
        # Memory
        for k,v in sorted(self.machine.memory.items()):
            self.screen.print_at(str(k) + ": " + str(v), st, line)
            line += 1
            if line > self.TUI_COLUMN_HEIGHT-1:
                line = 1
                st = st + self.TUI_VAR_COL_WIDTH        

    def drawWindow(self):
        if self.screen.has_resized():
            with ManagedScreen() as newScreen:
                self.screen = newScreen
                self.resizeTUI()
        
        # Print instruction list
        self.drawInstructionList()
        # Print registers content
        self.drawRegAndMem()
        # Print statistics 
        self.drawStatistics()
        # Print output
        self.drawPrintLog()
        # Draw input field if needed
        if self.machine.waitingForInput:
            self.screen.move(0,self.TUI_SCREEN_HEIGHT-1)
            self.screen.draw(self.TUI_SCREEN_WIDTH-1, self.TUI_SCREEN_WIDTH-1, char=' ', bg=self.TUI_CURSOR)
            self.screen.print_at("Input: " + self.input, 1, self.TUI_SCREEN_HEIGHT-1, bg = self.TUI_CURSOR)

        self.screen.refresh()
        self.screen.clear_buffer(fg=self.TUI_NORMAL, bg=self.TUI_BACKGROUND, attr=A_NORMAL, 
            w=self.screen.width, h=self.screen.height)    


    def run(self):
        # Main loop 
        while not self.quit: 
            # Handle keyboard events
            ev = self.screen.get_key()
            self.handleEvent(ev)
            # Autostep handling 
            if self.autoStep:
                if self.machine.instructionCounter in self.breakpoints:
                    self.autoStep = False
                else:
                    self.machine.step()
                    if self.machine.waitingForInput:
                        self.autoStep = False 
                    if self.machine.hasHalted:
                        self.autoStep = False
                    continue
            if self.focusLocked:
                self.cursorPos = self.machine.instructionCounter
            
            if self.machine.waitingForInput and not self.inputNeeded:
                self.inputNeeded = True
                self.inputAccepted = False 

            if self.machine.waitingForInput and self.inputAccepted:
                try:
                    v = int(self.input) 
                    self.machine.input(v)
                    self.inputAccepted = False 
                    self.inputNeeded = False
                    self.input = ""
                except Exception as e:
                    self.string = ""
            if self.machine.waitingForInput:
                self.inputAccepted = False 

            self.drawWindow()

    def generateOutput(self, inputFile):
        while not self.machine.hasHalted:
            self.machine.step()

            
def startDebuger():
    if len(sys.argv) < 2:
            print("Input file not specyfied!")
            return
    f = open(sys.argv[1])
    instructions = f.read().split('\n')

    with ManagedScreen() as screen:
        deb = Debuger(instructions,screen)
        deb.run()

startDebuger()