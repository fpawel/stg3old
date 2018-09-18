object Form1: TForm1
  Left = 192
  Top = 124
  Width = 870
  Height = 640
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object panelStatus: TPanel
    Left = 0
    Top = 564
    Width = 854
    Height = 18
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object Splitter2: TSplitter
      Left = 150
      Top = 0
      Width = 4
      Height = 18
      Cursor = crHSplit
    end
    object Splitter3: TSplitter
      Left = 304
      Top = 0
      Width = 3
      Height = 18
      Cursor = crHSplit
    end
    object Splitter4: TSplitter
      Left = 457
      Top = 0
      Width = 4
      Height = 18
      Cursor = crHSplit
    end
    object Panel9: TPanel
      Left = 0
      Top = 0
      Width = 150
      Height = 18
      Align = alLeft
      Alignment = taLeftJustify
      BevelInner = bvLowered
      BevelOuter = bvNone
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object Panel10: TPanel
      Left = 154
      Top = 0
      Width = 150
      Height = 18
      Align = alLeft
      Alignment = taLeftJustify
      BevelInner = bvLowered
      BevelOuter = bvNone
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object Panel11: TPanel
      Left = 307
      Top = 0
      Width = 150
      Height = 18
      Align = alLeft
      Alignment = taLeftJustify
      BevelInner = bvLowered
      BevelOuter = bvNone
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
    object Panel12: TPanel
      Left = 461
      Top = 0
      Width = 393
      Height = 18
      Align = alClient
      Alignment = taLeftJustify
      BevelInner = bvLowered
      BevelOuter = bvNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
    end
  end
  object grd1: TStringGrid
    Left = 0
    Top = 0
    Width = 854
    Height = 564
    Align = alClient
    BorderStyle = bsNone
    Color = 14540253
    ColCount = 1
    DefaultRowHeight = 25
    FixedCols = 0
    FixedRows = 0
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goEditing]
    ParentFont = False
    PopupMenu = PopupMenu1
    TabOrder = 1
    OnGetEditText = grd1GetEditText
    OnSelectCell = grd1SelectCell
    ColWidths = (
      64)
  end
  object edGrd: TEdit
    Left = 59
    Top = 7
    Width = 98
    Height = 26
    BevelInner = bvNone
    BorderStyle = bsNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    Text = 'edGrd'
    Visible = False
    OnExit = edGrdExit
    OnKeyDown = edGrdKeyDown
    OnKeyPress = edGrdKeyPress
  end
  object MainMenu1: TMainMenu
    Left = 8
    Top = 8
    object menuAddSlave: TMenuItem
      Caption = #1044#1086#1073#1072#1074#1080#1090#1100
      OnClick = menuAddSlaveClick
    end
    object menuDelSlave: TMenuItem
      Caption = #1059#1076#1072#1083#1080#1090#1100
      OnClick = menuDelSlaveClick
    end
    object N3: TMenuItem
      Caption = #1057#1054#1052' '#1087#1086#1088#1090
      OnClick = N3Click
    end
    object N5: TMenuItem
      Caption = #1055#1086#1080#1089#1082' '#1087#1086' '#1072#1076#1088#1077#1089#1089#1072#1084
      OnClick = N5Click
    end
    object N2: TMenuItem
      Caption = #1050#1086#1085#1089#1086#1083#1100
      OnClick = N2Click
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 40
    Top = 8
    object N6: TMenuItem
      Caption = #1044#1086#1073#1072#1074#1080#1090#1100
      OnClick = menuAddSlaveClick
    end
    object N7: TMenuItem
      Caption = #1059#1076#1072#1083#1080#1090#1100
      OnClick = menuDelSlaveClick
    end
  end
end
