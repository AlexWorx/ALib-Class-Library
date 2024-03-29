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

  Attention:
    The author of this file had a lot of troubles in creating this script. The solution so
    far might be improvable.
    It seems that many IDEs, e.g. Eclipse CDT or CLion do not support the use of 'children' very well
    (as of August 2016 versions). As soon as child values are returned, the 'main' pretty
    printing result is not shown.  It is not very probable that this is a fault of this script,
    because this is also true for official pretty printer scripts of standard c++ container classes
    when used with these IDEs: The objects main value is just not shown with these IDEs.
    Update 2023/01/30: It is still an issue in CLion as tracked here:
        https://youtrack.jetbrains.com/issue/CPP-15217

    Therefore, this script is configurable. Configuration is made by adding global symbols
    to (the debug version) of the inferior (the library or executable to debug). This script
    checks if these symbols exists. The symbols are:
    - ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN
      If this symbol is defined, the pretty printers of this script never return any children.
      Instead, the children's names and values are just appended to the pretty-print result
      of the variable itself.

    - ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES
      If this symbol is defined, then pointer types are treated just as normal types.
      This is dangerous. For example, with CLion V. 2016.2.1, this symbol must only
      be set if ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN is set as well. If not, the whole debug system might
      crash and debugging is not possible anymore.

    To set the symbols, just add a global int value, in one of your C++ files (conditionally, only in
    debug mode). E.g.

        #if defined( ALIB_DEBUG )

        int ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN;
        int ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES

        #endif

        int main( int argc, char **argv )
        {
            ...
            ...

    Alternatively, for CMake users, the provided ALib/ALox CMake scripts create corresponding CMAke
    cache variables that allow to switch this feature.

    The symbols control the global script variables
    - cfg_suppress_children  and
    - cfg_find_pointers which
    are defined below. Instead of compiling the symbols into your software, you can also modify
    the initial value directly in this script. Their default is "-1" which makes the script
    search for the symbols on the first use.

    Currently, in file alib.cpp these variables are set for different IDEs.

    This is all quite ugly, but currently we do not find a different solution.


  Types supported so far:
    - All sorts of ALib string types,
    - strings::util::Tokenizer
    - system::Directory, system::CalendarDate
    - time::Ticks
    - threads::Thread, threads::ThreadLock, threads::ThreadLockNR,
    - lox::core::Logger, lox::core::Domain, lox::core::LoggerData

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

# For explanation see notes above!
cfg_suppress_children= -1

# For explanation see notes above!
cfg_find_pointers= -1

global cfg_alib_detected
global cfg_alib_character_is_wide
global cfg_alib_wchar_size



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

#----- Compare two ALib strings  ------
def areEqualStrings(strObject1, strObject2, width):
    if width == 0:
        if cfg_alib_character_is_wide:
            width= -1
        else:
            width=  1

    if width == -1:
        width= cfg_alib_wchar_size

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
#        0: default width
#       -1: default wide width (either 2 or 4)
def getASString(strObject):
    return getASString(strObject, 0)

def getASString(strObject, width):
    if   str(width) == "char":
        width= int(0)
    elif str(width) == "wchar_t":
        width= int(-1)
    elif str(width) == "char16_t":
        width= int(2)
    elif str(width) == "char32_t":
        width= int(4)

    if width == 0:
        if cfg_alib_character_is_wide:
            width= -1
        else:
            width=  1

    if width == -1:
        width= cfg_alib_wchar_size

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

    if not areEqualStrings( name, typeName, 0 ):
        result+= " (" + getASString( typeName, 1 ) + ")"
    result+= " [" + str(value["CntLogs"] ) + "]"

    return result


#
# ############################## The Pretty Printer class ##############################
#
def dbgDumpPythonObject(obj):
  for attr in dir(obj):
    print("  obj.%s = %r" % (attr, getattr(obj, attr)))


class ALibPrinter(object):
    ''' Generic printer taking the readily made variables result and children '''

    def __init__( self, pResult, pChildren ):
        self.result=        pResult
        self.childrenDict=  pChildren

    def to_string(self):
        return self.result

    def children(self):
        #print("------------------------------------ " + str(self.childrenDict))
        if self.childrenDict is None:
            self.childrenDict= [ ("No Children With This Pretty Printer" , "./."   ) ]
        return self.childrenDict


glbALibPrinterWithOutChildrenBlocker= False
class ALibPrinterWithOutChildren(object):
    ''' Printer that does not define the 'children' method, to avoid problems with
    some IDEs '''

    def __init__( self, pResult, pChildren ):
        self.result=        pResult
        self.childrenDict=  pChildren

    def to_string(self):
        result= self.result
        global glbALibPrinterWithOutChildrenBlocker
        if glbALibPrinterWithOutChildrenBlocker is False:
            if self.childrenDict is not None:
                result= str(result) + " | {"
                glbALibPrinterWithOutChildrenBlocker= True
                isFirst= True
                for t in self.childrenDict:
                    if not isFirst:
                        result+= ", "
                    isFirst= False
                    result+= t[0] +'=' + str(t[1])
                result+= "}"
                glbALibPrinterWithOutChildrenBlocker= False
        return result

    #def display_hint (self):
    #    return 'string'

#
# ############################## ALIB Printer ##############################
#
def lookup_pp_alib_and_alox( value  ):
    "Looks and implements ALib printers"

    # get character width configuration
    global cfg_alib_character_is_wide
    global cfg_alib_wchar_size
    global cfg_alib_detected
    if cfg_alib_detected is False:
        cfg_alib_detected = True
        print ( 'ALib: Detecting global symbol "ALIB_PRETTY_PRINTERS_DEFAULT_CHAR_IS_WIDE"...')
        if  gdb.lookup_global_symbol( "ALIB_PRETTY_PRINTERS_DEFAULT_CHAR_IS_WIDE" ) is None:
            print ( '      ... not found')
            cfg_alib_character_is_wide=  False
        else:
            print ( '      ... found')
            cfg_alib_character_is_wide=  True

        print ( 'ALib: Detecting global symbol "ALIB_PRETTY_PRINTERS_WCHAR_SIZE_IS_4"...')
        if  gdb.lookup_global_symbol( "ALIB_PRETTY_PRINTERS_WCHAR_SIZE_IS_4" ) is None:
            print ( '      ... not found')
            cfg_alib_wchar_size=  2
        else:
            print ( '      ... found')
            cfg_alib_wchar_size=  4



    typeNameOrig=  str(value.type.name )               # This is not used yet, but may be helpful to identify "custom pod-types"
    typeName    =  str(value.type.strip_typedefs() )




    if typeName.startswith("const "):
        typeName= typeName[6:]

    # not alib?
    if not typeName.startswith( "alib::"):
        return None
    typeName= typeName[6:]

    print("----ALib Type: " + typeNameOrig + " -> " +typeName )
    #print( "ALib: Lookup for type >%s< " % type )


    # pointer / reference
    global cfg_find_pointers
    if cfg_find_pointers == -1:
        print ( 'ALib: Detecting global symbol "ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES"...')
        if  gdb.lookup_global_symbol( "ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES" ) is None:
            print ( 'ALib: ... not found')
            cfg_find_pointers=  0
        else:
            print ( 'ALib: ... found')
            cfg_find_pointers=  1


    if cfg_find_pointers == 1 and typeName.endswith(" *"):
        typeName= typeName[0:-2]


    if typeName.endswith(" &"):
        typeName= typeName[0:-2]


    # declare result and children for our pretty printer
    result=     None
    children=   None


    try:
        ################################## ALib ##################################
        if typeName.startswith( ''):
            #typeName= typeName[5:]


            #------------------------------------- strings ---------------------------------
            if typeName.startswith( 'strings::' ):
                typeName= typeName[9::]

                #--------------------- char -----------------------
                if typeName.startswith("TString<"):
                    cType= typeName[8:typeName.find(">")]
                    asBuffer= getASString( value, cType )
                    result= "[" + str(value["length"]) + '] "'  + asBuffer + '"'

                    children=  [ ("buffer" , value["buffer"]   ),
                                 ("length" , value["length"]   )   ];


                elif typeName.startswith("TCString<"):
                    cType= typeName[9:typeName.find(">")]
                    result= "[" + str(value["length"]) + '] "'  + getASString( value, cType ) + '"'

                elif typeName.startswith("TSubstring<"):
                    cType= typeName[11:typeName.find(">")]
                    result= "[" + str(value["length"]) + '] "'  + getASString( value, cType ) + '"'


                elif typeName.startswith("TAString<"):
                    cType= typeName[9:typeName.find(">")]
                    result= "[" + str(value["length"]) +"/"
                    capacity= value["capacity"]
                    if capacity < 0:
                        result+=  str(-capacity) + "(Ext)"
                    else:
                        result+= str(capacity)
                    result+= '] "'  + getASString( value, cType ) + '"'

                    children= [  ( "alib::strings::TString<"+cType+">", value.cast(gdb.lookup_type("alib::strings::TString<char>") ) )
                                ,( "capacity" , value["capacity"] )
                              ];
                    try:
                        children.append( ("OTW_ReplaceExternalBuffer" , value["ALIB_OTW_ReplaceExternalBuffer"] ) )
                    except:
                        pass

                elif typeName.startswith("TLocalString<"):
                    cType= typeName[13:typeName.find(",")]
                    result= "[" + str(value["length"]) +"/"
                    capacity= value["capacity"]
                    if capacity < 0:
                        result+=  str(-capacity) +"(LS)"
                    else:
                        result+= str(capacity)  +"(Replaced!)"
                    result+= '] "'  + getASString( value, cType ) + '"'

                    children= [  ( "alib::strings::TAString<"+cType+">", value.cast(gdb.lookup_type("alib::strings::TAString<char>") ) )
                                ,( "capacity" , value["capacity"] )
                                 ];


                elif typeName.startswith("util::TTokenizer<"):
                    cType= typeName[17:typeName.find(">")]
                    result= 'Actual="'  + getASString( value["Actual"]  ,cType ) + \
                             '" Rest="' + getASString( value["Rest"]    ,cType ) + '"' + \
                             " Delim='" + chr(value["delim"]) + "'"

                else:  print("ALib Type without Pretty Printer: " + typeName )



            #--------------------------------- System ---------------------------------
            elif typeName.startswith( 'lang::system::' ):
                typeName= typeName[8::]

                if typeName == "Directory":
                    result= getASString( value["Path"] )



                elif typeName == "CalendarDate":
                    result=          str(   value["stamp"] >> 12       )               \
                            + '/'  + str( ( value["stamp"] >>  8) & 15 )               \
                            + '/'  + str( ( value["stamp"] >>  3) & 31 )               \
                            + ' (' + getDayOfWeekString( value["stamp"] & 7 )  + ')'

                elif typeName == "CalendarDateTime":
                    result=   '{}/{:02}/{:02} ({}) {}:{:02}:{:02} {}ms'.format(
                                          value["Year"       ]              , \
                                     int( value["Month"      ] )            , \
                                     int( value["Day"        ] )            , \
                      getDayOfWeekString( value["DayOfWeek"  ] & 7 )        , \
                                          value["Hour"       ]              , \
                                     int( value["Minute"     ] )            , \
                                     int( value["Second"     ] )            , \
                                     int( value["Millisecond"] )            )


            #--------------------------------- Time ---------------------------------
            elif typeName.startswith( 'time::' ):
                typeName= typeName[6::]

                if typeName == "Ticks":
                    result= getDurationString( value["stamp"]["__d"]["__r"] )

                elif typeName.endswith("Ticks>::Duration"):
                    result= getDurationString( value["span"]["__r"] )

                elif typeName == "DateTime":
                    result= getDurationString( value["stamp"]["__d"]["__r"] )

                elif typeName.endswith("DateTime>::Duration"):
                    result= getDurationString( value["span"]["__r"] )

                elif typeName == "StopWatch":
                    result=  "Cnt="  +               str( value["cntSamples"] )
                    result+= " SUM=" + getDurationString( value["sum"]["span"]["__r"] )
                    result+= " MIN=" + getDurationString( value["min"]["span"]["__r"] )
                    result+= " MAX=" + getDurationString( value["max"]["span"]["__r"] )
                    result+= " AVG=" + getDurationString( int(value["sum"]["span"]["__r"]) / int(value["cntSamples"])  )

            #--------------------------------- Memory ---------------------------------
            elif typeName.startswith( 'bitbuffer::' ):
                typeName= typeName[8::]

                if typeName == "BitBufferBase::Index":
                    result= "data[" + str(value["pos"]) +"]." + str(value["bit"] )

                elif typeName == "BitWriter":
                    result = "{:039_b}".format(int(value["word"]))
                    bitIdx= int( value["idx"]["bit"] )
                    bitIdx+= int(bitIdx / 4)
                    result= result[0:39-bitIdx] + "<" + result[39-bitIdx:]
                    result+= " Index=" + str(value["idx"]["pos"]) +"/" + str(value["idx"]["bit"] )

                elif typeName == "BitReader":
                    result = "{:039_b}".format(int(value["word"]))
                    bitIdx= int( value["idx"]["bit"] )
                    bitIdx+= int(bitIdx / 4)
                    result =  result[bitIdx:] + "<"
                    result+= " Index=" + str(value["idx"]["pos"]) +"/" + str(value["idx"]["bit"] )



            #--------------------------------- Threads ---------------------------------
            elif typeName.startswith( 'threads::' ):
                typeName= typeName[9::]
                if typeName == "ThreadLockNR":
                    tnrIsAcquired= value["dbgIsAcquiredBy"]["_M_thread"]
                    if tnrIsAcquired == 0 :
                        result= "Unlocked"
                    else:
                        result= "Locked (by internal thread ID: " + str(tnrIsAcquired) + ")"


                elif typeName == "ThreadLock":
                    tnrCntLock= value["cntAcquirements"]
                    if tnrCntLock == 0:
                        result= "Unlocked"

                    elif tnrCntLock == 1:
                        result= "Locked"

                    elif tnrCntLock < 0:
                        result= "Illegal State. cntAcquirements=" + str(tnrCntLock)

                    else:
                        result= "Locked (" + str(tnrCntLock) +")"

                    if value["safeness"] == 1:
                        result+= " (Unsafe mode!)"
                    else:
                        if tnrCntLock >= 1:
                            result+=  ", Owner: " + "#" + str(value["owner"]["_M_thread"]) + ' "'


#                    children= [ ("cntAcquirements"         , value["cntAcquirements"]         ),
#                                ("owner"                   , value["owner"]                   ),
#                                ("mutex"                   , value["mutex"]                   ),
#                                ("acquirementSourcefile"   , value["acquirementSourcefile"]   ),
#                                ("acquirementLineNumber"   , value["acquirementLineNumber"]   ),
#                                ("acquirementMethodName"   , value["acquirementMethodName"]   )   ];

                elif typeName == "Thread":
                    result =  "#" + str(value["id"]) + ' "' + getASString( value["name"] )
                    result+=  '" (Alive/' if (value["isAliveFlag"]   != 0) else '" (Not alive/'
                    result+=  "User)"    if (value["id"]             >  0) else "System)"


            ################################## ALox ##################################
            elif typeName.startswith( 'lox::' ):
                typeName= typeName[5:]


                #---------------------------- Verbosity ------------------------------
                if typeName == "Verbosity":
                    result= getVerbosityString( value )

                #---------------------------- Loggers ------------------------------
                elif typeName == "detail::Logger"                 or \
                     typeName == "detail::textlogger::TextLogger" or \
                     typeName == "loggers::ConsoleLogger"       or \
                     typeName == "loggers::MemoryLogger"        or \
                     typeName == "loggers::AnsiLogger"          or \
                     typeName == "loggers::AnsiConsoleLogger":

                    result= getLoggerDescription( value )

                #---------------------------- Domain ------------------------------
                elif typeName == "core::Domain":

                    # get absolute domain path
                    result= ""
                    domain= value
                    omitFirstSlash= domain["Parent"] != 0
                    while True:
                        if omitFirstSlash == False:
                            result=   "/" + result
                        omitFirstSlash= False

                        if domain["Name"]["length"] != 0:
                            result= getASString( domain["Name"], 1 ) + result
                        domain= domain["Parent"]
                        if domain == 0:
                            break

                    result+= "  [" + str(value["CntLogCalls"]) + "]"

                    children= [   #("Name"                     , value["Name"]              ) ,
                                   ("Parent"                   , value["Parent"]            ) ,
                                   ("SubDomains"               , value["SubDomains"]        ) ,
                                   ("Data"                     , value["Data"]              ) ,
                                   ("FullPath"                 , value["FullPath"]          ) ,
                                   ("CntLogCalls"              , value["CntLogCalls"]       ) ,
                                   ("ConfigurationAlreadyRead" , value["ConfigurationAlreadyRead"] )   ]


                #---------------------------- Domain::LoggerData ------------------------------
                elif typeName == "core::Domain::LoggerData":
                    result = "<"            + getVerbosityString(   value["LoggerVerbosity"]   )
                    result+= ", "           + getLoggerDescription( value["Logger"]            )
                    result+= ", priority="  + str(                  value["Priority"]          )
                    result+= ">["           + str(                  value["LogCallsPerDomain"] ) + "]"

                    #children= [ ("Name"                     , value["Name"]              ) ,
                    #            ("Parent"                   , value["Parent"]            ) ,
                    #            ("SubDomains"               , value["SubDomains"]        ) ,
                    #            ("Data"                     , value["Data"]              ) ,
                    #            ("FullPath"                 , value["FullPath"]          ) ,
                    #            ("CntLogCalls"              , value["CntLogCalls"]       ) ,
                    #            ("ConfigurationAlreadyRead" , value["ConfigurationAlreadyRead"] )   ];


    except Exception as e:
        print ("\nALib PrettyPrinters: Exception occurred: %s" % str(e))
        if result != None and len(result) > 0:
            result= "<Pretty Printer Exception. Unfinished result='%s'>" % result
        else:
            result= "<Pretty Printer Exception>"
        #pass


    # now return a printer
    if result is None:
        return None

    if children is None:
        return ALibPrinterWithOutChildren( result, children )

    global cfg_suppress_children
    if cfg_suppress_children == -1:
        print ( 'ALib: Detecting global symbol "ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN"...')
        if  gdb.lookup_global_symbol( "ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN" ) is None:
            print ( 'ALib: ... not found')
            cfg_suppress_children=  0
        else:
            print ( 'ALib: ... found')
            cfg_suppress_children=  1

    if cfg_suppress_children == 0:
        return ALibPrinter( result, children )
    else:
        return ALibPrinterWithOutChildren( result, children )


    return None

def add_printers(obj):
    "Register ALib/ALox pretty printer lookup methods with."

    print( 'ALib: Registering Pretty Printers for ALib', True  )
    gdb.printing.register_pretty_printer(obj, lookup_pp_alib_and_alox, replace=True)


####################################################################################################
### main entry point
####################################################################################################

# invoked from .gdbinit ?
cfg_alib_detected= False
if gdb.current_objfile() is None:

    # import adpp main module
    try:
        import alibpp
    except Exception as e:
        gdb.write("Exception occurred: %s" % str(e) )
        raise Exception("Seems like this script is included in file '.gdbinit', but the propper import path is not set. ")


else:
    # import adpp main module
    try:
        import alibpp
    except:
        # In  /usr/lib/gcc/x86_64-pc-linux-gnu/5.4.0/libstdc++.so.6.0.21-gdb.py  some code
        # is included that inserts the module path to find the library from the object file name
        # not supported today....
        print ( __file__)

        raise Exception( "Can't find module 'addp.py'. Exiting" )


alibpp.add_printers(gdb.current_objfile())

##### For debugging: starts our test program.
##### (We just get more info when running this in a stand-alone GDB session)
#gdb.execute ( "file Debug/ALib_ALox_UT" )
#gdb.execute ( "break lox.cpp:1656" )
#gdb.execute ( "run" )
