# always process the following tex files
@default_files = ("art.tex");

# gen synctex
$latex = "latex -synctex=1 %O %S";

# gen .pdf on osx with pstopdf
$pdf_mode = "2";
$ps2pdf = "pstopdf %S -o %D";

# open .ps and .pdf with skim
$ps_previewer = "open -a Skim";
$pdf_previewer = "open -a Skim";
