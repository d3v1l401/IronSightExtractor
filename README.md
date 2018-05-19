# IronSight Extractor

![https://puu.sh/Ap861/4707ddf74e.png](https://puu.sh/Ap861/4707ddf74e.png)

The game had a QuickBMS script working for the old version, it wasn't supporting the decryption of the files because encryption method was unknown; but not anymore, now.

This version is extraction only for now, will be progressively extended based on my chances of survival during this exams session and my success in Linear Algebra.

## Installation

Windows:

```powershell
./ISExtractor.exe script.wpg
```

## Usage example

Simply pass the file path to extract, it will automatically create directories and files extracted and decrypted in the working directory.

## Development setup

You need 
* Crypto++ 7.0
* The installed game, or a single archive (one is included in the Debug directory)

## Release History

* 0.0.1
    * PoC - Extraction fully working

## Meta

d3vil401 – [d3vsite.org](https://d3vsite.org/) – d3vil401@d3vsite.org

Distributed under the GPL license. See ``LICENSE`` for more information.

[Me on GitHub](https://github.com/d3v1l401/)

## Contributing

1. Fork it (<https://github.com/d3v1l401/IronSightExtractor/fork>)
2. Create your feature branch, specify what you're doing (`git checkout -b updatedKeys`)
3. Commit your changes (`git commit -am 'Updated keys to 2020 game version'`)
4. Push to the branch (`git push origin updatedKeys`)
5. Create a new Pull Request

## Miscellanous

[Check the Wiki](https://github.com/d3v1l401/IronSightExtractor/wiki) for the file structure and details.