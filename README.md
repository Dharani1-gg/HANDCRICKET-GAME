# Hand Cricket Game

A fun and interactive Odd or Even Cricket Game built with HTML, CSS, and JavaScript.

## Project Structure

```bash
HANDCRICKET-GAME/
├── index.html      # Main HTML file with game structure
├── styles.css      # All CSS styles and animations
├── script.js       # Game logic and functionality
└── README.md       # Project documentation
```

## Files Description

### index.html

- Contains the HTML structure of the game
- Includes game sections: toss, choice selection, gameplay, and controls
- Links to external CSS and JavaScript files
- Uses semantic HTML elements for better accessibility

### styles.css

- Contains all CSS styles for the game
- Includes theme variables for easy customization
- Responsive design elements
- Animations (shake, rainbow background, fade effects)
- Button styles and layout components

### script.js

- Main game logic implemented as a JavaScript class (`HandCricketGame`)
- Handles game state management
- Implements different difficulty levels
- Screenshot and sharing functionality
- Theme switching capabilities

## Game Features

- **Coin Toss**: Start the game with a head or tails choice
- **Multiple Themes**: Dark, Light, Purple, and secret Rainbow theme
- **Difficulty Levels**: Easy, Medium, Hard AI,also nightmare level opponents
- **Screenshot Sharing**: Capture and share game results
- **Responsive Design**: Works on different screen sizes
- **Sound Effects**: Audio feedback for game events
- **Keyboard Controls**: Enter to play, Shift to reset

## How to Play

1. Open `index.html` in a web browser
2. Choose HEAD or TAILS for the coin toss
3. Select whether to bat or bowl first
4. Enter numbers 0-10 to play each round
5. Try to avoid matching the computer's number when batting
6. Try to match the computer's number when bowling

## Technical Features

- **Object-Oriented JavaScript**: Clean class-based architecture
- **Separation of Concerns**: HTML structure, CSS styling, and JS logic in separate files
- **Modern CSS**: CSS custom properties (variables) for theming
- **Responsive Design**: Mobile-friendly layout
- **Progressive Enhancement**: Works without JavaScript for basic functionality

## Browser Compatibility

- Modern browsers supporting ES6+ features
- HTML5 Canvas support required for screenshot functionality
- Web Share API for enhanced sharing (optional)

## Installation

1. Clone or download the project files
2. Open `index.html` in a web browser
3. No additional setup required - it's a client-side only application

## Development

To modify the game:

- Edit `styles.css` for visual changes
- Modify `script.js` for game logic changes
- Update `index.html` for structural changes

The code is well-commented and organized for easy maintenance and extension.
