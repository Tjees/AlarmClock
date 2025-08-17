<h1>Alarm Klok</h1>
Dit is de documentatie voor de RTOS alarm klok, de documentatie bestaat momenteel uit de huidige onderwerpen:<p>

- Use Case Diagram
- Use Case Bescrhijvingen
- Object Model
- Taakstructurering
- STDs

De volgende onderwerpen worden nog toegevoegd:
- Activity Diagrammen
- Klassendiagram

<h2>Use Case Diagram</h2>
Voor deze opdracht zijn een aantal use cases opgesteld, binnen dit use case diagram zijn vier use cases te zien. Deze zullen en het volgende onderwerp verder worden toegelicht:

![Diagram](./Use%20Case%20Diagram.png)

<h2>Use Case Beschrijvingen</h2>
Elke use case in het use case diagram is uitgewerkt in een use case beschrijving, de beschrijvinge bevat voornamelijk:

- Wat is de preconditie, welke actie moet de gebruiker uitgevoerd hebben om deze use case te beginnen.
- Wat zijn de stappen die de gebruiker volgt binnen de use case en welke acties worden er uitgevoerd.
- Wat is uiteindelijk de eindsituatie of het doel wat behaald wordt met de use case.

Dit zijn de beschrijvingen van de use cases voor de alarm klok, copilot is gebruik om het een en ander van word naar markdown te veranderen dus verder is het ook nog terug te vinden in en word document:

### Instellingen aanpassen

| **Veld**         | **Beschrijving**                                                                 |
|------------------|----------------------------------------------------------------------------------|
| Naam             | Instellingen aanpassen                                                           |
| Doel             | Het kunnen aanpassen van instellingen                                            |
| Precondities     | –                                                                                |
| Postconditie     | De geselecteerde instellingen kunnen worden aangepast                            |
| Beschrijving     | - Wacht tot de instel knop wordt ingedrukt op de remote<br>- Instellingen worden getoond op het display<br>- Tijdmenu knop → tijd instellen<br>- Alarmmenu knop → alarm instellen<br>- Menu knop → terug naar operationele taken |
| Uitzonderingen   | –                                                                                |

---

### Tijd aanpassen

| **Veld**         | **Beschrijving**                                                                 |
|------------------|----------------------------------------------------------------------------------|
| Naam             | Tijd aanpassen                                                                   |
| Doel             | Het kunnen aanpassen van de weergegeven tijd                                     |
| Precondities     | De instelling “tijd instellen” is geselecteerd                                   |
| Postconditie     | De gewenste tijd is ingesteld                                                    |
| Beschrijving     | - Huidige tijd wordt weergegeven<br>- Wacht op knop/signaal van remote<br>- “1” knop → uren aanpassen<br>- “2” knop → minuten aanpassen<br>- Omhoog knop → +1 (uren/minuten)<br>- Omlaag knop → -1 (uren/minuten)<br>- Menu knop → terug naar operationele taken<br>- Goto 1 |
| Uitzonderingen   | –                                                                                |

---

### Alarm aanpassen

| **Veld**         | **Beschrijving**                                                                 |
|------------------|----------------------------------------------------------------------------------|
| Naam             | Alarm aanpassen                                                                  |
| Doel             | Het kunnen aanpassen van het alarm                                               |
| Precondities     | De instelling “alarm instellen” is geselecteerd                                  |
| Postconditie     | Het gewenste alarm is ingesteld                                                  |
| Beschrijving     | - Huidig alarm wordt weergegeven<br>- Wacht op knop/signaal van remote<br>- “1” knop → uren aanpassen<br>- “2” knop → minuten aanpassen<br>- Omhoog knop → +1 minuut<br>- Omlaag knop → -1 minuut<br>- Menu knop → terug naar operationele taken<br>- Goto 1 |
| Uitzonderingen   | –                                                                                |

---

### Operationele taken uitvoeren

| **Veld**         | **Beschrijving**                                                                 |
|------------------|----------------------------------------------------------------------------------|
| Naam             | Operationele taken uitvoeren                                                     |
| Doel             | Het bijhouden van de tijd en het alarm                                           |
| Precondities     | –                                                                                |
| Postconditie     | De tijd is verhoogd en het alarm gaat af                                         |
| Beschrijving     | - Huidige tijd wordt weergegeven<br>- Wacht op timer<br>- Timer af → tijd +1 minuut<br>- Tijd = alarm → buzzer geluid<br>- Goto 1 |
| Uitzonderingen   | –                                                                                |

<h2>Activity Diagrams</h2>
Voor elke use case is een activity diagram gemaakt (momenteel de twee hoofd use cases).

De volgende activity diagrams zijn gemaakt:

<h3>AD OperationControl</h3>

![Diagram](./AD%20OperationControl.png)

<h3>AD InstelControl</h3>

![Diagram](./AD%20InstelControl.png)

<h2>Object Model</h2>
Op basis van de use cases zijn een aantal objecten opgenomen in een object model, deze is als volgt:

![Diagram](./Object%20Model.png)

Hieronder heb ik een tabel met de verschillende objecten en functies:

### Objecten in object model:

| **Objectnaam**         | **Type**       | **Functie / Beschrijving**                                                                 |
|------------------------|----------------|---------------------------------------------------------------------------------------------|
| `Display`              | Boundary       | Toont de huidige tijd of instellingen aan de gebruiker.                                    |
| `Buzzer`               | Boundary       | Maakt geluid wanneer het alarm afgaat.                                                     |
| `NecReceiver`          | Control        | Ontvangt en decodeert signalen van de afstandsbediening.                                   |
| `PauseDetector`        | Control        | Detecteert pauzes in het signaal om commando’s van de afstandsbediening te interpreteren.  |
| `InstelControl`        | Control        | Beheert het instellingenmenu en stuurt keuzes van de gebruiker door naar de juiste logica. |
| `TijdInstelControl`    | Control        | Verwerkt de logica voor het aanpassen van de huidige tijd.                                 |
| `AlarmInstelControl`   | Control        | Verwerkt de logica voor het instellen of aanpassen van het alarm.                          |
| `OperationControl`     | Control        | Houdt de tijd bij en activeert het alarm wanneer nodig.                                    |
| `Time`                 | Entity         | Wordt gebruikt voor het opslaan van de systeem en alarm tijd.                              |

<h2>Taakstructurering</h2>

## Eerste versie van taken

| **Object**           | **Taaksoort**         | **Periode** | **Deadline** | **Prioriteit** |
|----------------------|------------------------|-------------|--------------|----------------|
| InstelControl        | Intern, Demand Driven  | –           | 30ms         | 2              |
| TijdInstelControl    | Intern, Demand Driven  | –           | 30ms         | 2              |
| AlarmInstelControl   | Intern, Demand Driven  | –           | 30ms         | 2              |
| OperationControl     | Intern, Demand Driven  | 1000ms      | 1000ms       | 3              |
| NecReceiver          | Intern, Demand Driven  | –           | 100µs        | 1              |
| PauseDetector        | Intern, Demand Driven  | –           | 100µs        | 1              |
| Display              | IO, Demand Driven      | 200ms       | –            | 3              |
| Buzzer               | IO, Demand Driven      | –           | 2100ms       | 3              |
| IR-Receiver          | IO, Demand Driven      | –           | 100µs        | 1              |

---

## Ontwerpoverwegingen

- **InstelControl, TijdInstelControl, AlarmInstelControl**  
  Deadline van 30ms gekozen voor voldoende responsiviteit zonder overbelasting.  
  Deze drie zijn sterk afhankelijk van elkaar en vormen samen het instellingenmenu.  
  Door control cohesie zijn ze samengevoegd tot één centrale taak.

- **OperationControl**  
  Periode en deadline van 1000ms omdat de tijd slechts elke seconde hoeft te worden geüpdatet.  
  Lagere prioriteit gekozen vanwege flexibiliteit en lage execution time.

- **NecReceiver & PauseDetector**  
  Deadline van 100µs gebaseerd op voorbeeld STDs.  
  Later omgezet naar interrupt-driven, waardoor polling vervalt en periode niet meer nodig is.

- **Display**  
  Periode van 200ms gekozen voor voldoende marge om informatie stabiel weer te geven.

- **Buzzer**  
  Deadline van 2.1s gekozen zodat het alarm 2s klinkt met extra marge voor toggling.  
  Geen vaste periode nodig door gebruik van PWM of piezo buzzer.

- **IR-Receiver**  
  Aanvankelijk opgenomen als gepollde taak. Hoort eingenlijk alleen bij pausedetector dus zouden deze ook samengevoegd kunnen worden. Later verwijderd door overstap naar interrupt-driven aanpak.

- **Control cohesie**  
  InstelControl fungeert als master van TijdInstelControl en AlarmInstelControl.  
  Samenvoegen vermindert context switching en vereenvoudigt de taakstructuur.

---

## Huidige taakstructuur

| **Object**           | **Taaksoort**         | **Periode** | **Deadline** | **Prioriteit** |
|----------------------|------------------------|-------------|--------------|----------------|
| InstelControl        | Intern, Demand Driven  | –           | 30ms         | 2              |
| OperationControl     | Intern, Demand Driven  | 1000ms      | 1000ms       | 3              |
| NecReceiver          | Intern, Demand Driven  | –           | 100µs        | 1              |
| PauseDetector        | Intern, Demand Driven  | –           | 100µs        | 1              |
| Display              | IO, Demand Driven      | 200ms       | –            | 3              |
| Buzzer               | IO, Demand Driven      | –           | 2s           | 3              |

---

Deze structuur maakt de applicatie overzichtelijk en efficiënt, met duidelijke scheiding tussen instellingen, operationele logica en IO-componenten.

<h2>STDs</h2>

De STDs vond ik nogal lastig om te maken omdat deze best nauwkeuring moesten zijn en states tegelijkertijd super duidelijk maar ook super vaag zijn voor mij. Momenteel heb ik de volgende STDs gemaakt voor de alarm klok. Deze STDs waren redelijk goed over te nemen en na wat klassen en references gezeur werkte de alarm klok ook met deze STDs.

Nu wil ik wel nog zeggen dat de STDs zeker niet "af" zijn, tenminste dat vind ik. De functionaliteit voor het prototype zit er in maar er kan nog veel aan verbeterd worden. Ik heb zelf ook nog comments erbij gezet, deze mogen genegeerd worden maar zouden wel interessant kunnen zijn.

De volgende STDs zijn gemaakt:

<h3>STD OperationControl</h3>

![Diagram](./STD%20OperationControl.png)

<h3>STD InstelControl</h3>

![Diagram](./STD%20InstelControl.png)

<h3>STD TijdInstelControl</h3>

![Diagram](./STD%20TijdInstelControl.png)

<h3>STD AlarmInstelControl</h3>

![Diagram](./STD%20AlarmInstelControl.png)

<h3>STD Display</h3>

![Diagram](./STD%20Display.png)

<h3>STD Buzzer</h3>

![Diagram](./STD%20Buzzer.png)