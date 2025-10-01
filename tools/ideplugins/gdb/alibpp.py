'''#################################################################################################
  gdb pretty printer python scripts for classes of ALib and ALox

  (c) 2015-2023 A-Worx GmbH, Germany
  Published under 'Boost Software License' (a free software license, see LICENSE.txt)

  This Python script implements "pretty printers" for the GNU Debugger 'gdb' as  suggested here:
    https://sourceware.org/gdb/onlinedocs/gdb/Writing-a-Pretty_002dPrinter.html


  Installation:
    Add the following lines to your "~/.gdbinit" file:

        python sys.path.insert(0, "ALOX_LIB_PATH/tools/ideplugins/gdb" )
        python import alibpp

    (where ALOX_LIB_PATH has to be replaced by the right directory path!)

#################################################################################################'''
import string
try:
    import gdb
    import gdb.printing
    import gdb.types
    import gdb.events
except:
    pass

import os
import os.path
import sys
import struct
import types
import re

#
# ############################ Working with the inferior #############################
#
class InferiorByteArray(object):
    def __init__(self, base, width, size):
        size*= width;
        self.data = gdb.selected_inferior().read_memory(base, size)
        self.width= width

    def get(self, offset = 0):
        if self.width == 1:
            return struct.unpack_from("b", self.data, offset    )[0]
        if self.width == 2:
            return struct.unpack_from("H", self.data, offset * 2)[0]
        return     struct.unpack_from("I", self.data, offset * 4)[0]


#
# ############################ ALIB String Tools #############################
#

#----- Compare two ALib strings  ------       <
def areEqualStrings(strObject1, strObject2, width):
    if width == 0:
        gdb.write( "Error: width=0 in areEqualStrings!\n" )
        return 0

    # compare length
    length1=     strObject1["length"]
    length2=     strObject2["length"]

    if length1 != length2:
        return 0
    length= int(length1)

    # compare addresses
    straddress1= strObject1["buffer"]
    straddress2= strObject2["buffer"]

    if straddress1 == straddress2:
        return 1
    if straddress1 == 0 or straddress2 == 0:
        return 0

    # compare contents
    byteArray1 = InferiorByteArray( straddress1, width, length)
    byteArray2 = InferiorByteArray( straddress2, width, length)
    for i in range(0, length ):
        b1=  byteArray1.get(i)
        b2=  byteArray2.get(i)
        if b1 != b2:
            return 0

    # equal
    return 1


#----- Get the string value of an ALib string  ------
# Width: 1, 2 or 4
def getASString(strObject, width):
    if width < 1 or width > 4:
        return "Error: width wrongly given"

    straddress= strObject["buffer"]
    length=     strObject["length"]

    if length < 0:
        return "<negative length>"

    if straddress == 0:
        return "<nulled>"

    if length == 0:
        return "<empty>"

    if length > 200:
        cut=    True
        length= 200
    else:
        cut=    False

    byteArray = InferiorByteArray( straddress, width, length )
    asBuffer= ""
    zeroFound= 0
    for i in range(0, length ):
        c=  byteArray.get(i)
        #c= int(b)
        if (c < 0):
            c=  c + 256
        if c == 0 and zeroFound == 0:
            asBuffer= "('\\0' at %i!) "  % i  + asBuffer
            zeroFound= 1

        elif (c == 0x5C):
            asBuffer+= r"\\"

        elif (c == 0x22):
            asBuffer+= r'\"'

        elif (c == 0x07):
            asBuffer+= r'\a'
        elif (c == 0x08):
            asBuffer+= r'\b'
        elif (c == 0x09):
            asBuffer+= r'\t'
        elif (c == 0x0a):
            asBuffer+= r'\n'
        elif (c == 0x0b):
            asBuffer+= r'\v'
        elif (c == 0x0c):
            asBuffer+= r'\f'
        elif (c == 0x0d):
            asBuffer+= r'\r'
        elif (c == 0x1b):
            asBuffer+= r'\e'


        elif (c >= 32) and (c <= 127):
            asBuffer+= chr(c)

        else:
            if c <= 0xFF:
                    asBuffer+= r'\x'
                    asBuffer+= format(c, '02x')
            elif c <= 0xFFFF:
                    asBuffer+= r'\u'
                    asBuffer+= format(c, '04x')
            else:
                    asBuffer+= r'\U'
                    asBuffer+= format(c, '08x')

    if cut:
        asBuffer+= "..."
    return  asBuffer

#
# ############################ ALox Tools #############################
#

def getVerbosityString( value ):
    if value == 0:
        return "Verbose"
    if value == 1:
        return  "Info"
    if value == 2:
        return  "Warning"
    if value == 3:
        return  "Error"
    if value == 4:
        return  "Off"

    return  "<unknown value (" + str(value) + ")>"

def getDayOfWeekString( value ):
    if value == 0:
        return "Sun"
    if value == 1:
        return  "Mon"
    if value == 2:
        return  "Tue"
    if value == 3:
        return  "Wed"
    if value == 4:
        return  "Thu"
    if value == 5:
        return  "Fri"
    if value == 6:
        return  "Sat"
    return "<unknown day>"

def getDurationString( value ):
    started= False
    iv= int( value )
    result= ""

    i= int(iv / (365*24*60*60*1000*1000*1000))
    if i > 0:
        started= True
        result= str(i) + "years "
    iv-= int(i * (365*24*60*60*1000*1000*1000))

    i= int(iv / (24*60*60*1000*1000*1000))
    if started or i > 0:
        started= True
        result+= str(i) + "days - "
    iv-= int(i * (24*60*60*1000*1000*1000))

    i= int(iv / (60*60*1000*1000*1000))
    if started or i > 0:
        started= True
        result+= format(i, '02') + ":"
    iv-= int(i * (60*60*1000*1000*1000))

    i= int(iv / (60*1000*1000*1000))
    if started or i > 0:
        started= True
        result+= format(i, '02') + ":"
    iv-= int(i * (60*1000*1000*1000))

    i= int(iv / (1000*1000*1000))
    if started or i > 0:
        started= True
        result+= format(i, '02') + " - "
    iv-= int(i * (1000*1000*1000))

    i= int(iv / (1000*1000))
    if started or i > 0:
        started= True
        result+= format(i, '03') + ","
    iv-= int(i * (1000*1000))

    i= int(iv / (1000))
    if started or i > 0:
        result+= format(i, '03') + ","
    iv-= int(i * (1000))

    result+= format(iv, '03') + "ns"
    return result


def getLoggerDescription(value):
    name=       value["Name"]
    typeName=   value["TypeName"]
    result= getASString( name, 1 )

    if not areEqualStrings( name, typeName, 1 ):
        result+= " (" + getASString( typeName, 1 ) + ")"
    result+= " [" + str(value["CntLogs"] ) + "]"

    return result


def get_variable_name(val):
    """
    Attempt to retrieve the name of the variable corresponding to the gdb.Value.

    Args:
        val (gdb.Value): The value being pretty-printed.

    Returns:
        str: The name of the variable, or None if not found.
    """
    frame = gdb.selected_frame()
    for sym in frame.block():
        if sym.is_variable or sym.is_argument:
            try:
                if frame.read_var(sym).address == val.address:
                    return sym.name
            except gdb.error:
                pass
    return "<var_not_found>"

def dbgDumpPythonObject(obj):
  for attr in dir(obj):
    print("  obj.%s = %r" % (attr, getattr(obj, attr)))

def extractCurlyBrackets(inputString):
    """
    Simple regex matcher that extracts the portion of a given string that is enclosed in
    curly brackets {}.

    Args:
        inputString (str): The string to parse.

    Returns:
        str: The extracted string. If no brackets are found, the original string.
    """
    match = re.search(r"{\s*(.*?)\s*}", inputString)
    if match:
        return match.group(1)

    return inputString

def extractQuotation(inputString):
    """
    Simple regex matcher that extracts the portion of a given string that is enclosed in
    qutoation marks "...".

    Args:
        inputString (str): The string to parse.

    Returns:
        str: The extracted string. If no quotation marks are found, the original string.
    """
    match = re.search(r"\"\s*(.*?)\s*\"", inputString)
    if match:
        return match.group(1)

    return inputString

def getPPValue(value):
    """
    Retrieve the pretty-printed value of a gdb.Value.

    Args:
        value (gdb.Value): The value to pretty-print.

    Returns:
        str: The pretty-printed string, or the raw string if no pretty-printer is found.
    """
    pp = gdb.default_visualizer(value)
    if pp:
        return pp.to_string()
    return str(value)  # Fallback to raw string representation


def invoke(value, method):
    """
    Invoke the method on the given object.
    Exceptions are not handled here. Thus the caller can generate a specific error message.
    Args:
        value (gdb.Value): The value that represents the object (for pointers see invokeOnPointer).
        method (str): The method (including call brackets) to invoke.

    Returns:
        gdb value: The result of the method invocation.
    """

    try:
        # Get the address of the value
        addr = value.address
        if not addr:
            # Compute the address using `&` explicitly
            addr = gdb.parse_and_eval(f"&({value})")

        # Step 3: Cast the address to the appropriate pointer type
        pointer_type = value.type.pointer()  # Create the pointer type (e.g., T*)
        ptr_value = addr.cast(pointer_type)

        # Step 4: Dereference the pointer and call the method
        evalStr=   "((" + str(pointer_type) +  ")"+str(int(ptr_value))+ ")->"+method
        print("EvalString: <" + evalStr+">")
        method_result = gdb.parse_and_eval(evalStr)
        return method_result

    except gdb.error as e:
        print( "Error invoking method: " + method )
        return None

def invokeOnPointer(value, method):
    """
    Invoke the method on the casted address of the pointer given with the gdb value.
    Exceptions are not handled here. Thus the caller can generate a specific error message.
    Args:
        value (gdb.Value): The value that represents the pointer.
        method (str): The method (including call brackets) to invoke.

    Returns:
        gdb value: The result of the method invocation.
    """

    address= value.dereference()
    type=  str(address.type.pointer())
    evalStr=   "((" + type +  f"){address.address})->{method}"
    #print(f"ALib: evalStr=<{evalStr}>")
    return gdb.parse_and_eval(evalStr)


class ALibPrinter:
    ''' Generic printer taking the readily made display string while keeping children '''

    def __init__( self, pDisplay, pValue ):
        self.display=  pDisplay
        self.value  =  pValue

    def to_string(self):
        return self.display

    def children(self):
        try:
            if not hasattr(self.value.type, "fields") or not self.value.type.is_complete():
                return
            fields = self.value.type.fields()
            if not fields:
                return
            for field in fields:
                yield field.name, self.value[field.name]
        except Exception as e:
            yield f"Error accessing children: {str(e)}", None

#
# ############################## ALIB Printer ##############################
#
class ALibPrinterSelector:
    name    = "ALibPrettyPrinter"
    enabled = True

    def __init__(self):
        pass

    def __call__(self, value):

        typeNameOrig=  str(value.type.name )               # This is not used yet, but may be helpful to identify "custom pod-types"
        typeName    =  str(value.type.strip_typedefs() )

        if typeName.startswith("const "):
            typeName= typeName[6:]

        # not alib?
        if not typeName.startswith( "alib::"):
            return None
        typeName= typeName[6:]

        #print("----ALib Type: " + typeNameOrig + " -> " +typeName )
        #print( "ALib: Lookup for type >%s< " % type )

        if typeName.endswith(" &"):
            typeName= typeName[0:-2]

        # declare result for our pretty printer
        display=     None

        try:
            stdCharWidth= gdb.lookup_type("alib::character").sizeof
            #------------------------------------- strings ---------------------------------
            if typeName.startswith( 'lang::' ):
                typeName= typeName[6::]

                if typeName == "CallerInfo":
                    display=         getPPValue(value["File"])
                    display+= ":"  + getPPValue(value["Line"])
                    display+= " (" + getPPValue(value["Func"]) + ")"
                    display+= " "  + getPPValue(value["TypeInfo"])
                    try:
                       display+= getPPValue(value["ThreadID"])
                    except:
                       pass

                #--------------------------------- System ---------------------------------
                elif typeName.startswith( 'system::' ):
                    typeName= typeName[8::]

                    if typeName == "Path":
                        cWidth= gdb.lookup_type("alib::system::PathCharType").sizeof
                        asBuffer= getASString( value, cWidth )
                        display= "[" + str(value["length"]) + '] "'  + asBuffer + '"'
                        if( cWidth != stdCharWidth ):
                            display+= " (PathCharType)"


                    elif typeName == "CalendarDate":
                        display=          str(   value["stamp"] >> 12       )               \
                                + '/'  + str( ( value["stamp"] >>  8) & 15 )               \
                                + '/'  + str( ( value["stamp"] >>  3) & 31 )               \
                                + ' (' + getDayOfWeekString( value["stamp"] & 7 )  + ')'

                    elif typeName == "CalendarDateTime":
                        display=   '{}/{:02}/{:02} ({}) {}:{:02}:{:02} {}ms'.format(
                                              value["Year"       ]              , \
                                         int( value["Month"      ] )            , \
                                         int( value["Day"        ] )            , \
                          getDayOfWeekString( value["DayOfWeek"  ] & 7 )        , \
                                              value["Hour"       ]              , \
                                         int( value["Minute"     ] )            , \
                                         int( value["Second"     ] )            , \
                                         int( value["Millisecond"] )            )



            #------------------------------------- strings ---------------------------------
            elif typeName.startswith( 'strings::' ):
                typeName= typeName[9::]

                if (    typeName.startswith("TString<")
                     or typeName.startswith("TCString<")
                     or typeName.startswith("TSubstring<") ):

                    cType = str(value.type.template_argument(0))
                    cWidth= gdb.lookup_type(cType).sizeof
                    asBuffer= getASString( value, cWidth )
                    display= "[" + str(value["length"]) + '] "'  + asBuffer + '"'
                    if( cWidth != stdCharWidth ):
                        display+= " (" + cType + ")"

                elif typeName.startswith("TAString<"):
                    cType = str(value.type.template_argument(0))
                    aType = str(value.type.template_argument(1))
                    cWidth= gdb.lookup_type(cType).sizeof
                    asBuffer= getASString( value, cWidth )

                    display= "[" + str(value["length"]) +"/"
                    capacity= value["capacity"]
                    if capacity < 0:
                        display+=  str(-capacity) + "(Ext)"
                    else:
                        display+= str(capacity)
                    display+= '] "'  + getASString( value, cWidth ) + '"'
                    if( cWidth != stdCharWidth ):
                        display+= " (" + cType + ")"

                elif typeName.startswith("TLocalString<"):
                    cType     = str(value.type.template_argument(0))
                    TCapacity = int(value.type.template_argument(1))
                    aType     = str(value.type.template_argument(2))
                    cWidth= gdb.lookup_type(cType).sizeof
                    asBuffer= getASString( value, cWidth )

                    display= "[" + str(value["length"]) +"/"
                    capacity= value["capacity"]
                    if capacity < 0:
                        display+=  str(-capacity) + "(LS)"
                    else:
                        display+= str(capacity)   + " (Replaced)"
                    display+= '] "'  + getASString( value, cWidth ) + '"'
                    if( cWidth != stdCharWidth ):
                        display+= " (" + cType + ")"


                elif typeName.startswith("util::TTokenizer<"):
                    cType = str(value.type.template_argument(0))
                    cWidth= gdb.lookup_type(cType).sizeof
                    display= 'Actual="'  + getASString( value["Actual"]  ,cWidth ) + \
                             '" Rest="' + getASString( value["Rest"]    ,cWidth ) + '"' + \
                             " Delim='" + chr(value["delim"]) + "'"




            #--------------------------------- Time ---------------------------------
            elif typeName.startswith( 'time::' ):
                typeName= typeName[6::]

                if typeName == "Ticks":
                    display= getDurationString( value["stamp"]["__d"]["__r"] )

                elif typeName.endswith("Ticks>::Duration"):
                    display= getDurationString( value["span"]["__r"] )

                elif typeName == "DateTime":
                    display= getPPValue( value["stamp"] )
                    match = re.search(r"\[\s*(.*?)\s*\]", display)
                    if match:
                        display= match.group(1)


                elif typeName.endswith("DateTime>::Duration"):
                    display= getDurationString( value["span"]["__r"] )

                elif typeName == "StopWatch":
                    display=  "Cnt="  +               str( value["cntSamples"] )
                    display+= " SUM=" + getDurationString( value["sum"]["span"]["__r"] )
                    display+= " MIN=" + getDurationString( value["min"]["span"]["__r"] )
                    display+= " MAX=" + getDurationString( value["max"]["span"]["__r"] )
                    if int(value["cntSamples"]) > 0:
                        display+= " AVG=" + getDurationString( int(value["sum"]["span"]["__r"]) / int(value["cntSamples"])  )

            #--------------------------------- BitBuffer ---------------------------------
            elif typeName.startswith( 'bitbuffer::' ):
                typeName= typeName[11::]
                if typeName == "BitBufferBase::Index":
                    display= "data[" + str(value["pos"]) +"]." + str(value["bit"] )

                elif typeName == "BitWriter":
                    display = "{:039_b}".format(int(value["word"]))
                    bitIdx= int( value["idx"]["bit"] )
                    bitIdx+= int(bitIdx / 4)
                    display= display[0:39-bitIdx] + "<" + display[39-bitIdx:]
                    display+= " Index=" + str(value["idx"]["pos"]) +"/" + str(value["idx"]["bit"] )

                elif typeName == "BitReader":
                    display = "{:039_b}".format(int(value["word"]))
                    bitIdx= int( value["idx"]["bit"] )
                    bitIdx+= int(bitIdx / 4)
                    display =  display[bitIdx:] + "<"
                    display+= " Index=" + str(value["idx"]["pos"]) +"/" + str(value["idx"]["bit"] )



            #--------------------------------- Threads ---------------------------------
            elif typeName.startswith( 'threads::' ):
                typeName= typeName[9::]
                if typeName == "Thread":
                    display =  "#" + str(value["id"]) + ' "' + getASString( value["name"], stdCharWidth )
                    display+=  '" State::'
                    state=  value["state"]
                    if state == 0:
                        display+=  "Unstarted"
                    elif state == 1:
                        display+=  "Started"
                    elif state == 2:
                        display+=  "Running"
                    elif state == 3:
                        display+=  "Done"
                    elif state == 4:
                        display+=  "Terminated"
                    else:
                        display+=  "PrettyPrinterUnknownState"

                    if( value["id"] < 0 ):
                        display+=  " (Non-ALib-Thread)"

                elif typeName == "DbgLockAsserter":
                    display= str(value["Name"])
                    strCntAcq= str(value["CntAcquirements"])
                    if strCntAcq != "0":
                        display+= " Acquired(" + strCntAcq + ")"
                    else:
                        display+= " Not Acquired"

                    #-- print to gdb console to allow source-code jumping --
                    display+= " (Check GDB Console for source-jumps)"
                    consoleOutput=  "DbgLockAsserter CallerInfos:"
                    consoleOutput+= "\n  AcqCI: " + str(value["AcqCI"]["File"])    + ":" + str(value["AcqCI"]["Line"])
                    consoleOutput+= "\n  RelCI: " + str(value["RelCI"]["File"])    + ":" + str(value["RelCI"]["Line"])
                    print(consoleOutput.replace('"', ""))


                elif typeName == "DbgSharedLockAsserter":
                    display= str(value["Name"])
                    strCntAcq= str(value["CntAcquirements"])
                    if strCntAcq != "0":
                        display+= " Acquired(" + strCntAcq + ")"
                    else:
                        display+= " Not acquired"
                        
                    strCntShrdAcq= extractCurlyBrackets(getPPValue(value["CntSharedAcquirements"]))
                    if strCntShrdAcq != "0":
                        display+= ", Shared-acquired(" + strCntShrdAcq + ")"
                    else:
                        display+= ", Not shared-acquired"

                    #-- print to gdb console to allow source-code jumping --
                    display+= " (Check GDB Console for source-jumps)"
                    consoleOutput=  "DbgLockAsserter CallerInfos:"
                    consoleOutput+= "\n   AcqCI: " + str(value["AcqCI"]["File"])    + ":" + str(value["AcqCI"]["Line"])
                    consoleOutput+= "\n   RelCI: " + str(value["RelCI"]["File"])    + ":" + str(value["RelCI"]["Line"])
                    consoleOutput+= "\n  SAcqCI: " + str(value["SAcqCI"]["File"])   + ":" + str(value["SAcqCI"]["Line"])
                    consoleOutput+= "\n  SRelCI: " + str(value["SRelCI"]["File"])   + ":" + str(value["SRelCI"]["Line"])
                    print(consoleOutput.replace('"', ""))


                elif (     typeName == "Lock"
                        or typeName == "TimedLock"
                        or typeName == "RecursiveLock"
                        or typeName == "RecursiveTimedLock"
                        or typeName == "SharedLock"
                        or typeName == "SharedTimedLock" ):
                    display= getPPValue(value["Dbg"])

                elif typeName == "DbgConditionAsserter":
                    display= getPPValue(value["Name"])
                    strCntWaiters= extractCurlyBrackets(getPPValue(value["CntWaiters"]))
                    if strCntWaiters != "0":
                        display+= ", Waiters: " + strCntWaiters
                    else:
                        display+= ", No waiters"

                    if int(value["Owner"]) != 0:
                        display+= ", Owner={" + getPPValue(value["Owner"].dereference()) + "}"

                    #-- print to gdb console to allow source-code jumping --
                    display+= " (Check GDB Console for source-jumps)"
                    consoleOutput=  "DbgConditionAsserter CallerInfos:"
                    consoleOutput+= "\n     AcqCI: " + str(value["AcqCI"]["File"])    + ":" + str(value["AcqCI"]["Line"])
                    consoleOutput+= "\n     RelCI: " + str(value["RelCI"]["File"])    + ":" + str(value["RelCI"]["Line"])
                    consoleOutput+= "\n    WaitCI: " + str(value["WaitCI"]["File"])   + ":" + str(value["WaitCI"]["Line"])
                    consoleOutput+= "\n  NotifyCI: " + str(value["NotifyCI"]["File"]) + ":" + str(value["NotifyCI"]["Line"])
                    print(consoleOutput.replace('"', ""))

                elif typeName.startswith("TCondition<"):   
                    display= getPPValue(value["Dbg"])
                    

            ################################## ALox ##################################
            elif typeName.startswith( 'lox::' ):
                typeName= typeName[5:]


                #---------------------------- Verbosity ------------------------------
                if typeName == "Verbosity":
                    display= getVerbosityString( value )

                #---------------------------- Loggers ------------------------------
                elif typeName == "detail::Logger"                 or \
                     typeName == "textlogger::TextLogger" or \
                     typeName == "loggers::ConsoleLogger"       or \
                     typeName == "loggers::MemoryLogger"        or \
                     typeName == "loggers::AnsiLogger"          or \
                     typeName == "loggers::AnsiConsoleLogger":

                    display= getLoggerDescription( value )

                #---------------------------- Domain ------------------------------
                elif typeName == "detail::Domain":

                    # get absolute domain path
                    display= ""
                    domain= value
                    omitFirstSlash= domain["Parent"] != 0
                    while True:
                        if omitFirstSlash == False:
                            display=   "/" + display
                        omitFirstSlash= False

                        if domain["Name"]["length"] != 0:
                            display= getASString( domain["Name"], 1 ) + display
                        domain= domain["Parent"]
                        if domain == 0:
                            break

                    display+= "  [" + str(value["CntLogCalls"]) + "]"


                #---------------------------- Domain::LoggerData ------------------------------
                elif typeName == "detail::Domain::LoggerData":
                    display = "<"            + getVerbosityString(   value["LoggerVerbosity"]   )
                    display+= ", "           + getLoggerDescription( value["Logger"]            )
                    display+= ", priority="  + str(                  value["Priority"]          )
                    display+= ">["           + str(                  value["LogCallsPerDomain"] ) + "]"

            ################################## Files ##################################
            elif typeName.startswith( 'files::' ):
                typeName= typeName[7:]


                #---------------------------- File ------------------------------
                if typeName == "File":
                    node= value["node"].dereference()
                    display= '"' + getPPValue(node["name"]["storage"]) + '"'
                    display+= " [" + getPPValue(node["qtyChildren"])
                    display+= "/" + getPPValue(node["data"]["size"])
                    display+= "/" + getPPValue(node["data"]["mDate"])
                    display+= "] (Children/Size/Modif.)"


            ################################## Config ##################################
            elif typeName.startswith( 'variables::' ):
                typeName= typeName[8:]


                #---------------------------- Variable ------------------------------
                if typeName == "Variable":
                    node= value["node"].dereference()
                    display= extractQuotation(getPPValue(node["name"]["storage"]))
                    if node["data"]["meta"] == 0:
                        display+= " [undeclared]"
                    else:
                        type= "Error calling VMeta::typeName()"
                        display+= " <"
                        try:
                            type= extractQuotation(getPPValue(invokeOnPointer(node["data"]["meta"], "typeName()")))
                        except Exception as e:
                            print("ALib: Exception occurred while trying to get typename: " + str(e))
                            pass
                        display+= type + ">"

                        # Check if variable is defined
                        if node["data"]["priority"] == 0:
                            display+= " [undefined]"
                        else:
                            # Get some type-specific values
                            varval= "Pretty printer error retrieving variable value"
                            display+= ' "'
                            try:
                                if type == "B":
                                    varval= getPPValue(invoke(value, "GetBool()"))
                                elif type == "I":
                                    varval= getPPValue(invoke(value, "GetInt()"))
                                elif type == "F":
                                    varval= getPPValue(invoke(value, "GetDouble()"))
                                elif type == "S":
                                    AStringPA= invoke(value, "GetString()")
                                    StringType= gdb.lookup_type("alib::String").reference()
                                    varval= getPPValue(AStringPA.cast(StringType))
                                elif type == "SV," or type == "SV;" :
                                    size= invoke(value, "GetStrings().size()")
                                    display+= " [" + str(size) + "]={"
                                    for i in range(int(size)):
                                        sref= invoke(value, f"GetString({i})")
                                        s = sref.cast(sref.type.target())
                                        display+= '"' + extractQuotation(getPPValue(s)) + '"'
                                        if i!= int(size)-1:
                                            display+= ", "
                                    display+= "}"
                                    varval= None

                                else:
                                    varval+= "Pretty Printer: unknown variable type, cannot evaluate"

                            except Exception as e:
                                print("ALib: Exception occurred while trying to get variable value: " + str(e))
                                pass
                            if varval != None:
                                display+= varval + '"'

                        
        except Exception as e:
            print ("\nALib PrettyPrinters: Exception occurred: %s" % str(e))
            if display != None and len(display) > 0:
                display= "<Pretty Printer Exception. Unfinished display='%s'>" % display
            else:
                display= "<Pretty Printer Exception>"
            #pass


        # now return a printer
        if display is None:
            return None

        return ALibPrinter( display, value )


####################################################################################################
### main entry point
####################################################################################################
gdb.printing.register_pretty_printer(gdb.current_objfile(), ALibPrinterSelector())


##### For debugging: starts our test program.
##### (We just get more info when running this in a stand-alone GDB session)
#gdb.execute ( "file Debug/ALib_ALox_UT" )
#gdb.execute ( "break lox.cpp:1656" )
#gdb.execute ( "run" )
