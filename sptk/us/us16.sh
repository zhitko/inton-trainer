#! csh -f
# ----------------------------------------------------------------- #
#             The Speech Signal Processing Toolkit (SPTK)           #
#             developed by SPTK Working Group                       #
#             http://sp-tk.sourceforge.net/                         #
# ----------------------------------------------------------------- #
#                                                                   #
#  Copyright (c) 1984-2007  Tokyo Institute of Technology           #
#                           Interdisciplinary Graduate School of    #
#                           Science and Engineering                 #
#                                                                   #
#                1996-2013  Nagoya Institute of Technology          #
#                           Department of Computer Science          #
#                                                                   #
# All rights reserved.                                              #
#                                                                   #
# Redistribution and use in source and binary forms, with or        #
# without modification, are permitted provided that the following   #
# conditions are met:                                               #
#                                                                   #
# - Redistributions of source code must retain the above copyright  #
#   notice, this list of conditions and the following disclaimer.   #
# - Redistributions in binary form must reproduce the above         #
#   copyright notice, this list of conditions and the following     #
#   disclaimer in the documentation and/or other materials provided #
#   with the distribution.                                          #
# - Neither the name of the SPTK working group nor the names of its #
#   contributors may be used to endorse or promote products derived #
#   from this software without specific prior written permission.   #
#                                                                   #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            #
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       #
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          #
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          #
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS #
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          #
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   #
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     #
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON #
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   #
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    #
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           #
# POSSIBILITY OF SUCH DAMAGE.                                       #
# ----------------------------------------------------------------- #


#########################################################################
#                                                                       #
#       Sampling rate conversion from 10|12 kHz to 16 kHz               #
#                                                                       #
#                                           1998    T.Kobayashi         #
#                                           2000.7  S.Sako              #
#                                                                       #
#########################################################################

set path    = ( /usr/local/SPTK/bin $path )
set libpath = '/usr/local/SPTK/share/SPTK'
set sptkver = '3.7'
set cvsid   = '$Id: us16.in,v 1.13 2013/12/16 09:02:05 mataki Exp $'

set cmnd = $0
set cmnd = $cmnd:t
set file

set itype = '+ff'
set otype = '+ff'

set iext = 10
set oext = 16

set stdinput = 1

@ n = 1
@ s = 1

@ i = 0
while($i < $#argv)
        @ i++
        switch($argv[$i])
        case -s:
                @ i++
                set iext = $argv[$i]
                breaksw
        case -h:
                goto usage
                breaksw
        default
                if( -d $argv[$i]) then
                        set destdir = $argv[$i]
                else if( -e $argv[$i]) then
                        set file = ( $file $argv[$i] )
                        set stdinput = -1
                else
                        if( ${#file} > 1 ) then
                            echo2 "${cmnd} : Too many input files!"
                            exit 1
                        endif
                        set outfile = $argv[$i]
                endif
                breaksw
        endsw
end
goto cnvt


cnvt:

switch ($iext)
        case 10:
                set usops = "-c ${libpath}/lpfcoef.5to8 -u 8 -d 5"
                breaksw
        case 12:
                set usops = "-c ${libpath}/lpfcoef.3to4 -u 4 -d 3"
                breaksw
        default
                goto usage
endsw

if( $stdinput == 1) then
        if ($?outfile) then
                if( -e $outfile) then
                        echo2 "${cmnd} : File $outfile exits!"
                        exit 1
                endif
                x2x $itype |\
                us  $usops |\
                x2x $otype > $outfile
        else
                x2x $itype |\
                us  $usops |\
                x2x $otype
        endif
        exit 0
endif

foreach infile ($file)
        if ($?destdir) then
                set outfile = $infile:t
                set outfile = {$destdir}/{$outfile:r}.$oext
        endif

        if ($?outfile) then
                if ( -e $outfile ) then
                        echo2 "${cmnd} : File $outfile exits!"
                        exit 1
                endif
                x2x $itype $infile |\
                us  $usops |\
                x2x $otype > $outfile
        else
                x2x $itype $infile |\
                us  $usops |\
                x2x $otype
        endif
end
exit 0

usage:

cat2 <<EOF

 us16 - up-sampling from 10 or 12 kHz to 16 kHz

  usage:
       $cmnd [ options ] [ infile ] [ outfile ]
       $cmnd [ options ] [ infile1 ] [ infile2 ] ... [ outdir ]

  options:
       -s s     : input sampling frequency 10|12 kHz         [10]
       -h       : print this message
  infile:
       data sequence                                         [stdin]
  stdout:
       data sequence                                         [stdout]
  notice:

 SPTK: version $sptkver
 CVS Info: $cvsid
  
EOF
exit 1

