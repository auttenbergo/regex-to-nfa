# Regex To NFA Algorithm

This branch has two codes, one for converting Regex to NFA, second for simulating the NFA.<br/>

# 1) Converting Regex to NFA:<br/>

## Input:<br/>
&nbsp;&nbsp;&nbsp;Regex<br/>
## Output:<br/>
&nbsp;&nbsp;&nbsp;Number of states, Number of recepient states, Number of transitions<br/>
&nbsp;&nbsp;&nbsp;Indexes of recepient states<br/>
&nbsp;&nbsp;&nbsp;For each state, print on the independent line following this schema:<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Number of transitions, Pairs where the first one is the symbol and the second one is state index to which it connects<br/>

# 2) Simulating The NFA:<br/>

## Input:<br/>
&nbsp;&nbsp;&nbsp;Word to simulate<br/>
&nbsp;&nbsp;&nbsp;NFA according to the previous task<br/>
## Output:<br/>
&nbsp;&nbsp;&nbsp;Printing side by side:<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Y(yes) if after reading one symbol NFA was in at least one recepient state<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;N(no)  if after reading one symbol NFA was not in any recepient state<br/>


/---------------------------------/<br/>
I had these tasks as homework in university(Free University). Tests are provided by my lecturer, Nikoloz Tsimakuridze
	