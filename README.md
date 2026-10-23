# `winter_is_coming`

Dokumentacja projektu OpenGL. Zastąp pola `<...>` informacjami właściwymi dla projektu i usuń nieużywane sekcje.

## Świat

- Autor: `Hanna Tovstokora`
- Biom: `Tundra`
- Ziarno generatora: `245114`
- Sposób wyznaczenia ziarna: `nie dotyczy`
- Krótki opis świata: `Kraina inspirowana Westerosem. Skuta lodem tundra. Słońce wisi nisko nad horyzontem.Krajobraz tworzą rówiny śnieżne, skaliste wzniesienia i formacje lodowe.`

### Opcjonalne rozszerzenie

- Nazwa: `<nazwa rozszerzenia albo brak>`
- Włączanie i wyłączanie: `<kontrolka lub klawisz>`
- Parametry: `<lista kontrolek>`
- Tryby diagnostyczne: `<lista>`
- Ograniczenia: `<opis albo brak>`

## Wymagania środowiskowe

Projekt wspiera:

- Windows albo Linux;
- OpenGL 4.5 Core Profile;
- GLSL 450;
- CMake 3.21 lub nowszy;
- kompilator obsługujący C++20.

Wymagany jest również Git do pobrania repozytorium.

## Szybki start

Po sklonowaniu repozytorium przejdź do jego głównego katalogu:

```text
git clone <adres-repozytorium>
cd <katalog-repozytorium>
cmake --list-presets
cmake --preset debug
cmake --build --preset debug --parallel
```

Na Windows te same kroki można wykonać skrótem:

```text
.\setup_project.bat
```

Bez argumentu skrypt używa presetu `debug`. Aby zbudować wariant Release,
podaj nazwę presetu:

```text
.\setup_project.bat release
```

Skrypt konfiguruje i buduje projekt, ale nie uruchamia testów.

Preset `debug` służy do codziennej pracy, diagnostyki i korzystania
z callbacku debugowego OpenGL.

Uruchom program:

```text
# Windows z generatorem Visual Studio
.\build\debug\src\Debug\OpenGLGP.exe

# Linux z generatorem jednokonfiguracyjnym
./build/debug/src/OpenGLGP
```

Wymagane argumenty lub znane ograniczenia: `<opis / brak>`.

Domyślne lokalizacje pliku wykonywalnego wynikają z użytego generatora:

- Debug, generator wielokonfiguracyjny: `build/debug/src/Debug/OpenGLGP.exe`;
- Debug, generator jednokonfiguracyjny: `build/debug/src/OpenGLGP`;
- Release, generator wielokonfiguracyjny: `build/release/src/Release/`;
- Release, generator jednokonfiguracyjny: `build/release/src/`.

Dodatkowy krok wymagany po zbudowaniu projektu: `<opis / brak>`.

## Struktura projektu i zasoby

- `src/framework/` — dostarczona infrastruktura okna, pętli i diagnostyki;
- `src/project/` — kod właściwy projektu;
- `res/` — shadery, modele i tekstury świata;
- `tests/` — testy kodu CPU.

Własne pliki `.c`, `.cpp`, `.h` i `.hpp` umieszczaj w `src/project/` albo
w jego podkatalogach. Zasoby porządkuj według typów:

- shadery w `res/shaders/`;
- tekstury w `res/textures/`;
- modele i ich materiały w `res/models/`.

CMake automatycznie wykrywa nowe pliki źródłowe w całym `src/` dzięki
`GLOB_RECURSE CONFIGURE_DEPENDS`. Nie dopisuj ich ręcznie do
`src/CMakeLists.txt` — po dodaniu pliku wystarczy ponownie zbudować projekt.
Zawartość `res/` jest kopiowana obok pliku wykonywalnego podczas budowania.

Nietypowe katalogi lub pliki: `<opis albo brak>`.

## Co zapewnia framework

Kod w `src/framework/` przygotowuje wspólną infrastrukturę aplikacji.
Student nie musi samodzielnie pisać:

- inicjalizacji i zamykania GLFW, GLAD oraz ImGui;
- tworzenia okna 1280×720 i kontekstu OpenGL 4.5 Core Profile;
- głównej pętli aplikacji, odpytywania zdarzeń i wymiany buforów;
- obliczania czasu klatki i numerowania klatek;
- pobierania rzeczywistego rozmiaru framebufferu i ustawiania viewportu;
- oczekiwania, gdy okno jest zminimalizowane i framebuffer ma rozmiar 0×0;
- debug contextu i callbacku komunikatów OpenGL w konfiguracji `debug`;
- sprawdzania wersji OpenGL/GLSL oraz podstawowych limitów GPU;
- integracji ImGui z GLFW i OpenGL.

Framework udostępnia również:

- `AssetLocator` — wyszukiwanie plików względem katalogu `res/`;
- `ShaderCompiler` — odczyt, kompilację shaderów i linkowanie programu wraz
  z pełnym komunikatem błędu; zwrócone uchwyty OpenGL nadal muszą być
  zwalniane przez kod studenta;
- `--diagnostics` — raport dostawcy, renderera, wersji OpenGL/GLSL, profilu,
  debug contextu, obsługi sRGB, timer queries, DSA i limitów zasobów;
- `--diagnostics-json <plik>` — zapis tego samego raportu do JSON;
- biblioteki GLFW, GLAD, GLM, ImGui, Assimp z importerami OBJ/FBX/glTF,
  stb_image i spdlog.

Framework nie implementuje elementów ocenianych w zadaniach. Student nadal
tworzy między innymi teren, bufory i VAO, kamerę, tekstury, import modeli,
graf sceny, oświetlenie, instancing, cienie i postprocessing. Kod dostarczony
w `src/framework/` nie jest zaliczany jako część pakietu Modern OpenGL.

## Punkt wejścia kodu projektu

Kod projektu należy rozwijać od klasy `src/project/Project`. Framework wywołuje
w każdej klatce:

```cpp
void update(const framework::FrameContext& frame);
void render(const framework::FrameContext& frame);
void drawGui();
```

`FrameContext` przekazuje:

- `deltaSeconds`, `elapsedSeconds` i `frameIndex`;
- szerokość i wysokość framebufferu;
- wskaźnik `GLFWwindow*` potrzebny do obsługi wejścia;
- flagi `uiWantsMouse` i `uiWantsKeyboard`, informujące, czy wejście jest
  aktualnie przechwycone przez ImGui.

## Sterowanie

- Ruch kamery: `W/A/S/D`
- Obrót kamery: `mysz (po kliknięciu okna)`
- Ruch góra/dół: `Space (góra), Left Ctrl (dół)`
- Przechwycenie/zwolnienie kursora: `kliknięcie w okno/Escape`
- Reset kamery: `R albo przycisk „Reset camera"`
	- Najważniejsze kontrolki ImGui: `<Amplitude>, <Frequency>, <Freq gain>, <Persistence>, <Octaves>, <Grid size>, <World size>, <Wireframe>, <Reset params>, <Camera speed>, <FOV>`
- Przełączniki diagnostyczne: `<Wireframe>, <Mesh>`

## Diagnostyka

### Diagnostyka środowiska

Raport środowiska można wyświetlić po zbudowaniu wariantu Debug:

```text
# Windows
.\build\debug\src\Debug\OpenGLGP.exe --diagnostics

# Linux
./build/debug/src/OpenGLGP --diagnostics
```

Raport musi potwierdzić OpenGL 4.5 Core Profile i GLSL 450.
Najważniejsze wartości z raportu:

- `GL_VENDOR`: `<wartość>`;
- `GL_RENDERER`: `<wartość>`;
- `GL_VERSION`: `<wartość>`;
- `GL_SHADING_LANGUAGE_VERSION`: `<wartość>`;
- profil i debug context: `<wartość>`;
- domyślny framebuffer sRGB: `<tak/nie>`;
- baseline OpenGL 4.5 Core/GLSL 450: `<spełniony/niespełniony>`.

Raport można również zapisać do pliku JSON:

```text
<ścieżka-do-programu>/OpenGLGP --diagnostics-json <plik>
```
 VAO: `terrain.VAO` — pole `Vertex Array Object`.
- VBO: `terrain.VBO` — wiersz `0`, stride 24 bajty.
- EBO: `terrain.EBO` — wiersz `Element`, indeksy 4-bajtowe.
- Atrybut 0 (`aPos`): 3 × float32, offset 0.
- Atrybut 1 (`aColor`): 3 × float32, offset 12.
- Liczba indeksów: 23814, typ `GL_UNSIGNED_INT`.

### Diagnostyka renderowania

- Tryb debugowy OpenGL: `włączony automatycznie w konfiguracji `debug`
  (framework tworzy kontekst z `GLFW_OPENGL_DEBUG_CONTEXT` i rejestruje
  `glDebugMessageCallback`). W konsoli widoczne są komunikaty o błędach
  shaderów i kontekstu.`
- Podglądy buforów i tekstur: `<kontrolki>`
- Markery RenderDoc: `<najważniejsze zdarzenia>`
- Liczniki CPU/GPU: `licznik Mesh w ImGui (wierzchołki, trójkąty,
  indeksy), licznik FPS z ImGui.`

## Budowanie Release i charakterystyka wydajności

Preset `release` jest zoptymalizowany i przeznaczony do końcowych pomiarów
wydajności CPU/GPU. Nie używaj konfiguracji `debug` do porównywania
wydajności.

```text
cmake --preset release
cmake --build --preset release --parallel
```

Konfiguracja pomiaru: `<sprzęt, rozdzielczość, VSync i parametry sceny>`.

- Wariant: `<np. 1 000 instancji>`
  - liczba draw calli: `<liczba>`
  - czas CPU: `<wynik i metoda agregacji>`
  - czas GPU: `<wynik i metoda>`
- Wariant: `<np. 10 000 instancji>`
  - liczba draw calli: `<liczba>`
  - czas CPU: `<wynik>`
  - czas GPU: `<wynik>`
- Wariant: `<np. 100 000 instancji>`
  - liczba draw calli: `<liczba>`
  - czas CPU: `<wynik>`
  - czas GPU: `<wynik>`

Wnioski: `<krótka interpretacja wąskiego gardła i porównania albo brak>`.

## Zasoby zewnętrzne

Modele, tekstury i inne zasoby potrzebne do uruchomienia projektu:

- `<ścieżka>` — `<przeznaczenie, źródło i licencja>`

Nietypowe informacje konieczne do poprawnego załadowania zasobów:
`<opis albo brak>`.

## Znane ograniczenia

- `<ograniczenie albo brak>`
