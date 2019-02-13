# Dictation

**Dictation** is a free desktop application for language learners to improve their listening and spelling skills, it's written in C++ using Qt framework. This application is capable of splitting the contents of a listening material into sentences. You can play each sentence separately, repeat it as you wish, and type the words you hear in text boxes.

![[Screenshot]][screenshot]

## Features

* **Speech segmentation**  
By clicking ![[Open]][open], you can select an audio file to play. The metadata of the chosen file will be displayed on the upper panel, and its contents will be split into segments of sentences, by detecting the silence between them, and listed on the lower panel in chronological order.  
The time interval of a segment is represented as a pair of timecodes that are formatted as _MINUTES:SECONDS.MILLISECONDS_.
* **Audio playback**  
You can click ![[Play]][play] or ![[Pause]][pause] to play or pause the playback, ![[Next]][next] and ![[Previous]][previous] allow you to skip between segments.  
The dark background of a segment indicates the segment which is currently being played.
* **Volume adjustment**  
The playback volume can be adjusted by dragging the slider, and you can click ![[Speaker]][speaker] and ![[Unmute]][speaker_x] to toggle mute.
* **Sentence repeat**  
If you like to repeat a single segment, click ![[Repeat]][repeat] of that segment to repeat it and it will repeat until you click ![[Stop Repeat]][stop_repeat] to stop it.
* **Full keyboard navigation and control**  
Keyboard shortcuts are provided so that you won't be distracted by mouse clicking while you are typing.
* **Autosave**  
The text you have typed and the settings you have configured will be restored when you restart the application.
* **Data export**  
You can export the text you have typed as an SRT file with a click on ![[Export]][export].

## Compilation Prerequisites

In order to build it, you must have Qt, version 5.11 or later, installed and a compiler with support for C++11 language features is required on your system.  
If you like to build it as a standalone executable, you may need to compile a static version of Qt libraries yourself.

## License

Copyright (C) 2019 Albert Hsu.

Licensed under the [GNU General Public License version 3](COPYING) or any later version.

[open]: images/markdown/folder.png
[export]: images/markdown/export.png
[speaker]: images/markdown/speaker-2.png
[speaker_x]: images/markdown/speaker-x.png
[play]: images/markdown/play.png
[pause]: images/markdown/pause.png
[repeat]: images/markdown/repeat.png
[stop_repeat]: images/markdown/stop-repeat.png
[previous]: images/markdown/previous.png
[next]: images/markdown/next.png
[screenshot]: images/markdown/screenshot.png
