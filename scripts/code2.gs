var sheet_id = "1fUnjaL6DI-ov1e9lRdRY6TXEd-iDfGpsTgr1hhGVfiw";
var ss = SpreadsheetApp.openById(sheet_id);
var sheet = ss.getSheetByName('final_eval');


function doGet(e) {
  var Action = String(e.parameter.Action);
  var Amount = String(e.parameter.Amount);
  var Balance = Number(e.parameter.Balance);
  sheet.appendRow([Action,Amount,Balance]);
}
