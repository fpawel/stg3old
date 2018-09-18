object FrmWait: TFrmWait
  Left = 248
  Top = 377
  BorderStyle = bsNone
  Caption = 'FrmWait'
  ClientHeight = 54
  ClientWidth = 267
  Color = 15855093
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 16
  object pb1: TProgressBar
    Left = 0
    Top = 39
    Width = 267
    Height = 15
    Align = alBottom
    Min = 0
    Max = 100
    Position = 50
    TabOrder = 0
    Visible = False
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 267
    Height = 39
    Align = alClient
    BevelOuter = bvNone
    Caption = #1055#1086#1078#1072#1083#1091#1081#1089#1090#1072', '#1087#1086#1076#1086#1078#1076#1080#1090#1077'...'
    Color = 15855093
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clMaroon
    Font.Height = -30
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
end
