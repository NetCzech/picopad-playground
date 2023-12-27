## A simple Virtual keyboard for the [Picopad](https://picopad.eu/en/) gaming console.

---

> <picture>
>   <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/Mqxx/GitHub-Markdown/main/blockquotes/badge/light-theme/info.svg">
>   <img alt="Info" src="https://raw.githubusercontent.com/Mqxx/GitHub-Markdown/main/blockquotes/badge/dark-theme/info.svg">
> </picture><br>
> &nbsp;
> &nbsp;
> &nbsp;
> 
> **CONTROL:**
>> **_Left key:_**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;move to the left
>
>> **_Right key:_**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;move to the right
>
>> **_Up key:_**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;move to the up
>
>> **_Down key:_**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;move to the down
>
>> **_A key:_**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;insert a letter
>
>> **_B key:_**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;enter

---

**:black_cat: NOTICE :black_cat:**

The app is still in development, this is an alpha version

---

TO-DO LIST:
- [ ] automatic character transition to the next line
- [ ] keyboard layout for special characters
- [ ] keyboard layout for accented characters

---

**⚠️ WARNING ⚠️**

Please DO NOT directly upload the build to Pico / Picopad. The build is specifically designed for the custom
bootloader, which will load it from the SD card into flash memory behind the main bootloader. You must upload the 
contents of the /build directory to an SD card.

Please note, the build does NOT include a BOOT2 section. Directly uploading the build to the Picopad / Pico will
brick your Pico / Picopad.
