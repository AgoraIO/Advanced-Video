import React from 'react';
import { HashRouter as Router, Route } from "react-router-dom";
import Home from './pages/Home';
import { MobxStoreProvider } from './store';
import RemoteControl from './pages/RemoteControl';



const App: React.FC = () => {
  return (
    <MobxStoreProvider>
      <Router>
        <Route path="/" exact component={Home}/>
        <Route path="/control" exact component={RemoteControl} />
      </Router>
    </MobxStoreProvider>
  );
}

export default App;
