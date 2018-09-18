object FormSearchDAU: TFormSearchDAU
  Left = 192
  Top = 124
  BorderStyle = bsToolWindow
  Caption = 'FormSearchDAU'
  ClientHeight = 185
  ClientWidth = 297
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  PixelsPerInch = 120
  TextHeight = 16
  object pnSearchDevice: TPanel
    Left = 0
    Top = 0
    Width = 289
    Height = 144
    Alignment = taLeftJustify
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      289
      144)
    object Label1: TLabel
      Left = 36
      Top = 3
      Width = 51
      Height = 16
      Caption = #1040#1076#1088#1077#1089#1089':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object Label2: TLabel
      Left = 18
      Top = 27
      Width = 72
      Height = 16
      Caption = #1054#1087#1088#1086#1096#1077#1085#1086':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 24
      Top = 51
      Width = 68
      Height = 16
      Caption = #1054#1089#1090#1072#1083#1086#1089#1100':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object Label4: TLabel
      Left = 24
      Top = 75
      Width = 73
      Height = 16
      Caption = #1054#1090#1074#1077#1090#1080#1083#1080':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object Label5: TLabel
      Left = 8
      Top = 99
      Width = 92
      Height = 16
      Caption = #1053#1077' '#1086#1090#1074#1077#1090#1080#1083#1080':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object ProgressBar1: TProgressBar
      Left = 0
      Top = 128
      Width = 289
      Height = 16
      Align = alBottom
      Min = 0
      Max = 1000
      TabOrder = 0
    end
    object Edit1: TEdit
      Left = 104
      Top = 3
      Width = 177
      Height = 20
      Anchors = [akLeft, akTop, akRight]
      BorderStyle = bsNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentColor = True
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
      Text = 'Edit1'
    end
    object Edit2: TEdit
      Left = 104
      Top = 27
      Width = 177
      Height = 20
      Anchors = [akLeft, akTop, akRight]
      BorderStyle = bsNone
      ParentColor = True
      ReadOnly = True
      TabOrder = 2
      Text = 'Edit1'
    end
    object Edit3: TEdit
      Left = 104
      Top = 51
      Width = 185
      Height = 20
      Anchors = [akLeft, akTop, akRight]
      BorderStyle = bsNone
      ParentColor = True
      ReadOnly = True
      TabOrder = 3
      Text = 'Edit1'
    end
    object Edit4: TEdit
      Left = 104
      Top = 75
      Width = 177
      Height = 20
      Anchors = [akLeft, akTop, akRight]
      BorderStyle = bsNone
      ParentColor = True
      ReadOnly = True
      TabOrder = 4
      Text = 'Edit1'
    end
    object Edit5: TEdit
      Left = 104
      Top = 99
      Width = 177
      Height = 20
      Anchors = [akLeft, akTop, akRight]
      BorderStyle = bsNone
      ParentColor = True
      ReadOnly = True
      TabOrder = 5
      Text = 'Edit1'
    end
  end
  object Button1: TButton
    Left = 192
    Top = 152
    Width = 99
    Height = 25
    Caption = #1055#1088#1077#1088#1074#1072#1090#1100
    TabOrder = 1
    OnClick = Button1Click
  end
end
