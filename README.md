# English-to-Erindin Translator

A C++ application that translates English phrases into **Erindin**, a constructed language (conlang) created by Frederico Schroeder. The translator respects the complex grammatical rules and word order of the Erindin language.

## Features

- **Intelligent Word Ordering**: Automatically rearranges English sentences to match Erindin grammar (Verb-Subject-Object structure)
- **Compound Word Handling**: Processes hyphenated compound words with proper morphological rules
- **Tense Management**: Handles past, present, and future tense markers
- **Negation Support**: Properly processes negative sentences with appropriate word placement
- **Database-Driven Translation**: Uses MySQL database for comprehensive dictionary lookup
- **Grammar Rule Application**: Implements complex Erindin grammatical rules including:
  - Definite article handling
  - Plural marker placement
  - Adjective positioning
  - Possessive pronoun rules

## Architecture

The project consists of three main components:

### Core Classes

- **`Database`**: Manages MySQL connection and dictionary lookups
- **`Processor`**: Handles sentence parsing, word ordering, and translation logic
- **`main.cpp`**: User interface and input processing

### Key Features

- **Word Order Transformation**: Converts English SVO to Erindin VSO structure
- **Morphological Processing**: Handles compound words, plurals, and adjectives
- **Tense Markers**: Processes `(f)` for future and `(p)` for past tense
- **Special Suffixes**: Supports various grammatical markers like `(v)`, `(s)`, `(o)`, `(adj)`, `(pl)`
- **Compound Words**: Able to produce compound words when using hyphenation for use in nicknaming, or name generation, like `red-hand`, `the silent-one`

## Prerequisites

- **Visual Studio 2022** (v143 toolset)
- **MySQL Server** (running on localhost:3306)
- **MySQL Connector/C++** (configured in project settings)
- **C++20** standard support


## Usage

1. **Run the executable**:
   ```bash
   ./x64/Debug/translator.exe
   ```

2. **Enter English sentences** with grammatical markers:
   - Use `(v)` for verbs: `"I(s) run(v) fast"`
   - Use `(s)` for subjects: `"The cat(s) sleeps(v)"`
   - Use `(o)` for objects: `"I(s) eat(v) the red(adj) apple(o)"`
   - Use `(f)` for future tense: `"I(s) go(v)(f)"`
   - Use `(p)` for past tense: `"I(s) went(v)(p)"`
   - Use `(adj)` for adjectives: `"The big(adj) house(o)"`
   - Use `(pl)` for plurals: `"Many cat(pl)"`


3. **View the Erindin translation** with proper grammar and word order

## Example

**Input**: `"The cat(s) eats(v) the fish(o)"`

**Output**: `Borda te kedrin te fisgrin`

**Input**: `"We(s) invoke(v) the spirit(pl) of the ancestor(pl) for guidance"`

**Output**: `Glóra vi mar andan mar ódifedrin kye lédan`

**Process**:
1. Word order transformation: `"eats(v) the cat(s) the fish(o)"`
2. Direct translation from database
3. Grammar rule application
4. Final Erindin output

## Database Schema

The application expects a MySQL database with the following structure:

```sql
CREATE DATABASE dictionary;
USE dictionary;

CREATE TABLE dict (
    ID INT PRIMARY KEY,
    erindin VARCHAR(255),
    english VARCHAR(255),
    type VARCHAR(50)
);
```

## Configuration

### Database Connection
Update the connection parameters in `Database.cpp`:
```cpp
c = d->connect("tcp://localhost:3306", "username", "password");
```

### MySQL Connector Paths
Modify `translator.vcxproj` to match your MySQL Connector installation:
- Include directories
- Library dependencies
- Runtime libraries

## Language Credits

**Full credit for the Erindin constructed language goes to Frederico Schroeder.**

This translator implements the grammatical rules and vocabulary of the Erindin language as designed by its creator.

## Development

### Project Structure
```
translator/
├── Database.h/cpp     # MySQL database interface
├── Processor.h/cpp    # Translation logic and grammar rules
├── main.cpp          # Application entry point
└── translator.vcxproj # Visual Studio project file
```

### Key Algorithms
- **Word Ordering**: Implements VSO sentence structure transformation
- **Compound Processing**: Handles hyphenated words with morphological rules
- **Grammar Rules**: Applies Erindin-specific grammatical constraints

## License

This personal project is for educational and linguistic research purposes only, for use as a third-party helper with phrase generation, and is not meant for distribution.

