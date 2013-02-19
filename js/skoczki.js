$(function() {
  
  $('.pawn').click(function(){
    		    
    // zaznaczamy aktywne pole
    $('.selected').removeClass('selected')
  $(this).addClass('selected')
  
  // sprawdz czy na polu jest pionek
  var x = $(this).find('img')
  
  // chodzi o ruch gdzie (tam)
  if(x.length == 0){
    var id_pola = $(this).attr('id').replace('field','')
    $('#where').val(id_pola)     
    $('#plansza').submit()    
  } 
  // ustrawienie ruchy skad
  else{          
    // ustawiamy ruch z pola
    var id_pola = $(this).attr('id').replace('field','')
    $('#from').val(id_pola)
  }
  
  
  
  })
	
	
})
