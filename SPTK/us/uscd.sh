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
#       Sampling rate conversion from 8|10|12|16 kHz to                 #
#                                                 11.025|22.05|44.1 kHz #
#                                                                       #
#                                           1998    T.Kobayashi         #
#                                           2000.7  S.Sako              #
#                                                                       #
#########################################################################

set path    = ( /usr/local/SPTK/bin $path )
set libpath = '/usr/local/SPTK/share/SPTK'
set sptkver = '3.7'
set cvsid   = '$Id: uscd.in,v 1.13 2013/12/16 09:02:05 mataki Exp $'

set cmnd = $0
set cmnd = $cmnd:t
set file

set itype = '+ff'
set otype = '+ff'

set iext = 10
set oext = 11.025

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
        case -S:
                @ i++
                set oext = $argv[$i]
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

if(`echo "$iext == 8" | bc -l`) then
                @ osr = 4
else if(`echo "$iext == 10" | bc -l`) then
                @ osr = 5
else if(`echo "$iext == 12" | bc -l`) then
                @ osr = 6
else if(`echo "$iext == 16" | bc -l`) then
                @ osr = 8
else
                goto usage
endif

if(`echo "$oext == 44" | bc -l`) then   
else if(`echo "$oext == 44.1" | bc -l`) then
#               set oext = 44
else if(`echo "$oext == 22" | bc -l`) then
                @ osr *= 2
else if(`echo "$oext == 22.05" | bc -l`) then
#               set oext = 22
                @ osr *= 2
else if(`echo "$oext == 11" | bc -l`) then
                if (`echo "$iext == 10 || $iext == 8" | bc -l`) then
                        @ osr *= 4
                else
                        goto usage
                endif
else if(`echo "$oext == 11.025" | bc -l`) then
#               set oext = 11
                if (`echo "$iext == 10 || $iext == 8" | bc -l`) then
                        @ osr *= 4
                else
                        goto usage
                endif
else 
                goto usage
endif

if( $stdinput == 1) then
        if ($?outfile) then
                if( -e $outfile) then
                        echo2 "${cmnd} : File $outfile exits!"
                        exit 1
                endif
                x2x $itype |\
                us -c ${libpath}/lpfcoef.2to3f -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.2to3s -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d 5 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d $osr |\
                x2x $otype > $outfile
        else
                x2x $itype |\
                us -c ${libpath}/lpfcoef.2to3f -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.2to3s -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d 5 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d $osr |\
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
                us -c ${libpath}/lpfcoef.2to3f -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.2to3s -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d 5 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d $osr |\
                x2x $otype > $outfile
        else
                x2x $itype $infile |\
                us -c ${libpath}/lpfcoef.2to3f -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.2to3s -u 3 -d 2 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d 5 |\
                us -c ${libpath}/lpfcoef.5to7 -u 7 -d $osr |\
                x2x $otype
        endif
end
exit 0

usage:

cat2 <<EOF

 uscd - up/down-sampling from 8, 10, 12, or 16 kHz to 11.025, 22.05, or 44.1 kHz

  usage:
       $cmnd [ options ] [ infile ] [ outfile ]
       $cmnd [ options ] [ infile1 ] [ infile2 ] ... [ outdir ]

  options:
       -s s     : input sampling frequency (8, 10, 12, 16 kHz) [10]
       -S s     : output sampling frequency 
                                      (11.025, 22.05, 44.1kHz) [11.025]
       -h       : print this message
  infile:
       data sequence                                           [stdin]
  stdout:
       data sequence                                           [stdout]
  notice:

 SPTK: version $sptkver
 CVS Info: $cvsid
  
EOF
exit 1
