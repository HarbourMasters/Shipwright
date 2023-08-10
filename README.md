![Ship of Harkinian](docs/shiptitle.darkmode.png#gh-dark-mode-only)
![Ship of Harkinian](docs/shiptitle.lightmode.png#gh-light-mode-only)

# Basic Information

This repository exists to distribute versions of the Ocarina of Time PC port Ship of Harkinian for ARM64 devices. Currently, only a single ARM64 AppImage is provided. It is optimized for the Raspberry Pi 4.

## OpenGL Support

When running SoH ARM64, it is recommended to run <code>MESA_GL_VERSION_OVERRIDE=3.1 ./[AppImage Path]</code> in a terminal to start it. If the program freezes when launching for you, this will run it.
This is due to the OpenGL ES drivers on the Raspberry Pi 4 reporting its OpenGL version as 2.1, when it is really capable of 3.1.

## Website

Official Website: https://www.shipofharkinian.com/

## Discord

Official Discord: https://discord.com/invite/shipofharkinian

<a href="https://github.com/Kenix3/libultraship/">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="./docs/poweredbylus.darkmode.png">
    <img alt="Powered by libultraship" src="./docs/poweredbylus.lightmode.png">
  </picture>
</a>
