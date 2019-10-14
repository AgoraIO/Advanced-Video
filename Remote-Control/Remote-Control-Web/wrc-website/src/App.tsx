import React from 'react';
import { HashRouter as Router, Route } from "react-router-dom";
import Home from "./pages/Home";
import RemoteControl from './pages/RemoteControl';
import { MobxStoreProvider } from './store';
import NotificationWrapper from './components/NotificationWrapper';


const App: React.FC = () => {
  return (
    <MobxStoreProvider>
      <NotificationWrapper>
        <Router>
          <Route path="/" exact component={Home} />
          <Route path="/s/:channel" component={RemoteControl}/>
        </Router>
      </NotificationWrapper>
    </MobxStoreProvider>
  );
};

export default App;