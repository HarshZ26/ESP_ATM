var sheet_id = "1fUnjaL6DI-ov1e9lRdRY6TXEd-iDfGpsTgr1hhGVfiw";
var ss = SpreadsheetApp.openById(sheet_id);
var sheet = ss.getSheetByName('final_eval');


function doGet(e){
  var read = e.parameters.read;
  if (read !== undefined){
    let u_name =  sheet.getRange('A2').getValue();
    let password =  sheet.getRange('B2').getValue();
    var result = u_name + " " + password;
    sheet.getRange('C2').setValue(result);
    return ContentService.createTextOutput(sheet.getRange('C2').getValue());
  }
}
