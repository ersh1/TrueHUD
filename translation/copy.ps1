# USER DEFINED
$outDir = "C:\Skyrim Mod Organizer\mods\TrueHUD\Interface\Translations"

$strings = @('czech', 'english', 'french', 'german', 'italian', 'japanese', 'polish', 'russian', 'spanish')

ForEach ($string in $strings)
{
    Copy-Item "TrueHUD_english.txt" -Destination "$outDir\TrueHUD_$string.txt"
}