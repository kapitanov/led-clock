$source_name = "marke_eigenbau_normal_8"
$out_name="default";
$from = 0
$till = 256

$source_name = "lucida_console_regular_10"
$out_name="monospace";
$from = 33
$till = 256


[void] [System.Reflection.Assembly]::LoadWithPartialName('System.drawing')
$bmp = [System.Drawing.Bitmap]::FromFile((get-item "./$source_name.PNG").FullName)
$xml = [xml] (gc "./$source_name.xml" -Encoding UTF8)

$dir = join-path (Resolve-Path "../../") "data/font/$out_name"
md $dir -Force | out-null
write-host "Generating font `"$out_name`": [ " -n
try {
    $xml.font.chars.char | % {
        $ch = [char]$_.id
    
        if ($ch -gt $from -and $ch -lt $till) {
            $w = [int] $_.rect_w
            $h = [int] $_.rect_h
            $ox = [int] $_.rect_x
            $oy = [int] $_.rect_y
        
            if ($h -gt 8) {
                $h = 8;
                $offset = 0
            }
            else {
                $offset = 7 - $h
            }  
    
            write-host "$ch" -n
            $lines = @()
                
            for ($y = 0; $y -lt (8 - $h); $y++) {
                $line = ""
                for ($x = 0; $x -lt $w; $x++) {
                    $line += "."
                }
                $lines += $line
            }
        
            
            for ($y = 0; $y -lt $h; $y++) {
                $line = ""
                for ($x = 0; $x -lt $w; $x++) {
                    $pix = $bmp.GetPixel($x + $ox, $y + $oy)
                    if ($pix.A -gt 127) {
                        $line += "#"
                    }
                    else {
                        $line += "."
                    }
                }
                $lines += $line
            }

            $filename = join-path $dir "$([int]$ch).dat"
            [System.IO.File]::WriteAllLines($filename, $lines)
        }
    }
}
finally {
    $bmp.Dispose()
}
write-host " ]"
exit

# $code = "./font_data.cpp"
# "#include `"font_data.h`"" > $code
# $indices = @{}
# gci $dir -Filter *.png | % {    
#     $ch = [int][System.IO.Path]::GetFileNameWithoutExtension($_.FullName)
#     write-host "char $ch"
#     $indices[$ch] = 0

#     $bmp = [System.Drawing.Bitmap]([System.Drawing.Bitmap]::FromFile($_.FullName))

#     "// CHAR $ch $($bmp.Width)x$($bmp.Height)" >> $code    
#     "unsigned char __font_glyph_$ch[] PROGMEM = {" >> $code
#     "    $($bmp.Width)," >> $code
#     $idx++
#     for($x = 0; $x -lt $bmp.Width; $x++) {
#         $line = ""
#         for($y = 0; $y -lt $bmp.Height; $y++) {
#             $pix = $bmp.GetPixel($x, $y)
#             if ($pix.R -gt 127) {
#                 $line += "0"
#             }
#             else {
#                 $line += "1"
#             }
#         }

#         "    0b$line," >> $code
#         $idx++
#     }
#     "};" >> $code
# }
# "" >> $code
# "bool find_glyph(char c, glyph_t &glyph)" >> $code
# "{" >> $code
# "  unsigned char* ptr;" >> $code
# "  switch(c) {" >> $code
# $indices.Keys | % {
#     "  $($_):" >> $code
#     "    ptr = __font_glyph_$ch;" >> $code
#     "    break;" >> $code
# }
# "  default:" >> $code
# "    return false;" >> $code
# "    break;" >> $code
# "  }" >> $code
# "  " >> $code
# "  glyph.ptr = ptr + 1;" >> $code
# "  glyph.width = (unsigned char)pgm_read_byte(ptr)" >> $code
# "  glyph.height = 8;" >> $code
# "}" >> $code

$indices = @{}
gci $dir -Filter *.png | % {    
    $ch = [int][System.IO.Path]::GetFileNameWithoutExtension($_.FullName)
    if ($ch -lt 256) {
        $code = "#include `"../font.h`"`n"
        $code += "`n"
        $indices[$ch] = 0

        $bmp = [System.Drawing.Bitmap]([System.Drawing.Bitmap]::FromFile($_.FullName))

        $code += "// CHAR $ch $($bmp.Width)x$($bmp.Height)`n"
        $code += "unsigned char __font_glyph_$ch[] PROGMEM = {`n"
        $code += "    $($bmp.Width),`n"
        $idx++
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            $line = ""
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pix = $bmp.GetPixel($x, $y)
                if ($pix.R -gt 127) {
                    $line += "0"
                }
                else {
                    $line += "1"
                }
            }

            $code += "    0b$line,`n"
            $idx++
        }
        $code += "};`n"
    
        $path = Join-Path (Get-Location) "./glyphs/glyph_$ch.cpp"
        [System.IO.File]::WriteAllText($path, $code, [System.Text.Encoding]::Default);

        $bmp.Dispose()
    }
}

$code = ""
$code += "#include `"font.h`"`n"
$code += "`n"
$indices.Keys | % {
    $code += "extern unsigned char __font_glyph_$_[] PROGMEM;`n"
}
$code += "`n"
$code += "bool find_glyph(char c, glyph_t &glyph)`n"
$code += "{`n"
$code += "  unsigned char* ptr;`n"
$code += "  switch(c) {`n"
$indices.Keys | % {
    $code += "  case $($_):`n"
    $code += "    ptr = __font_glyph_$_;`n"
    $code += "    break;`n"
}
$code += "  default:`n"
$code += "    return false;`n"
$code += "    break;`n"
$code += "  }`n"
$code += "`n"
$code += "  glyph.ch = c;`n"
$code += "  glyph.ptr = ptr + 1;`n"
$code += "  glyph.width = (unsigned char)pgm_read_byte(ptr);`n"
$code += "  glyph.height = 8;`n"
$code += "}`n"

$path = Join-Path (Get-Location) "./font.cpp"
[System.IO.File]::WriteAllText($path, $code, [System.Text.Encoding]::Default);
