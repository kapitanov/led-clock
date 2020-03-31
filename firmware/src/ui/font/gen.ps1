[void] [System.Reflection.Assembly]::LoadWithPartialName('System.drawing')
$encoding = [System.Text.Encoding]::GetEncoding("windows-1251")

function GetByteCode([char] $ch) {
    return $encoding.GetBytes("$ch")[0]
}

function GenerateFont($source_name, $out_name) {
    $bmp = [System.Drawing.Bitmap]::FromFile((get-item "./$source_name.PNG").FullName)
    $xml = [xml] (gc "./$source_name.xml" -Encoding UTF8)

    $dir = join-path (Resolve-Path "../../../") "data/font/$out_name"
    md $dir -Force | out-null
    write-host "Generating font `"" -n
    write-host "$out_name" -n -f cyan
    write-host "`":"

    $chars = @($null) * 256

    try {
        $xml.font.chars.char | % {
            $ch = [char]$_.id
            $code = GetByteCode($ch)

            if ($chars[$code] -ne $null) {
                return
            }
    
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

            $chars[$code] = @{
                Char     = $ch;
                Code     = $code;
                FileName = (join-path $dir "$($code).dat");
                Lines    = $lines;
            }
        }
    }
    finally {
        $bmp.Dispose()
    }

    write-host "  |" -n -f yellow
    for ($i = 0; $i -lt 16; $i++) {
        write-host "$(([int]$i).ToString("X01")) " -n
    }
    write-host "|"-f yellow
    write-host "--+--------------------------------+"-f yellow

    $row = 0
    for ($i = 0; $i -lt $chars.Count; $i++) {
        if ($i % 16 -eq 0) {
            if ($i -ne 0) {
                write-host "|"-f yellow
            }
            write-host "$(([int]$row).ToString("X01")) " -n
            write-host "|" -n -f yellow
            $row++
        }

        $x = $chars[$i]
        if ($x -eq $null) {
            write-host "$([char]0x00B7) " -n -f red
            continue
        }
        write-host "$($x.Char) " -n -f green
        [System.IO.File]::WriteAllLines($x.FileName, $x.Lines)
    }
    write-host "|" -f yellow
    write-host "--+--------------------------------+" -f yellow
}


GenerateFont "marke_eigenbau_normal_8" "default"
GenerateFont "lucida_console_regular_10" "monospace"