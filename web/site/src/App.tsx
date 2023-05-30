import React from 'react';
import './App.css';

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <h3>Carro Detector de Obstáculos</h3>
        <h5>USJT - Engenharia da Computação - 2023.1</h5>
        <div className='equipe'>
          <strong>Equipe: </strong>
            Aquiles Colares de Oliveira - 820144933 &diams; Guilherme de Morais Cavalcante - 819154676 &diams; 
            Gustavo Adolfo Alencar - 820271356 &diams; Henrique Manuel dos Santos Costa - 819120677 &diams; 
            Rogério dos Santos Cruz - 822166361          
        </div>
      </header>

      <main>
        <canvas id="canvas" width="200" height="100" className="canvas-mapa"> 
          Se seu navegador não suportar HTML5 você verá esta mensagem. 
        </canvas> 
      </main>
    </div>
  );
}

export default App;
