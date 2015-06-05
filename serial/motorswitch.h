switch(Steps){
   case 0:
     digitalWrite(PB0, LOW); 
     digitalWrite(PB1, LOW);
     digitalWrite(PB2, LOW);
     digitalWrite(PB3, HIGH);
   break; 
   case 1:
     digitalWrite(PB0, LOW); 
     digitalWrite(PB1, LOW);
     digitalWrite(PB2, HIGH);
     digitalWrite(PB3, HIGH);
   break; 
   case 2:
     digitalWrite(PB0, LOW); 
     digitalWrite(PB1, LOW);
     digitalWrite(PB2, HIGH);
     digitalWrite(PB3, LOW);
   break; 
   case 3:
     digitalWrite(PB0, LOW); 
     digitalWrite(PB1, HIGH);
     digitalWrite(PB2, HIGH);
     digitalWrite(PB3, LOW);
   break; 
   case 4:
     digitalWrite(PB0, LOW); 
     digitalWrite(PB1, HIGH);
     digitalWrite(PB2, LOW);
     digitalWrite(PB3, LOW);
   break; 
   case 5:
     digitalWrite(PB0, HIGH); 
     digitalWrite(PB1, HIGH);
     digitalWrite(PB2, LOW);
     digitalWrite(PB3, LOW);
   break; 
     case 6:
     digitalWrite(PB0, HIGH); 
     digitalWrite(PB1, LOW);
     digitalWrite(PB2, LOW);
     digitalWrite(PB3, LOW);
   break; 
   case 7:
     digitalWrite(PB0, HIGH); 
     digitalWrite(PB1, LOW);
     digitalWrite(PB2, LOW);
     digitalWrite(PB3, HIGH);
   break; 
   default:
     digitalWrite(PB0, LOW); 
     digitalWrite(PB1, LOW);
     digitalWrite(PB2, LOW);
     digitalWrite(PB3, LOW);
   break; 
